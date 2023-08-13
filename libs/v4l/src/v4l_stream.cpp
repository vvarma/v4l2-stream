#include "v4l/v4l_stream.h"

#include <fmt/format.h>
#include <linux/videodev2.h>
#include <spdlog/spdlog.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <optional>
#include <vector>

#include "util.h"
#include "v4l/v4l_caps.h"
#include "v4l/v4l_device.h"
#include "v4l/v4l_exception.h"
#include "v4l/v4l_frame.h"

namespace v4s {

namespace internal {

struct Buffer {
  typedef std::shared_ptr<Buffer> Ptr;
  void *start;
  size_t length;
  Buffer(int fd, int offset, size_t length);
  ~Buffer();
};

std::vector<std::vector<internal::Buffer::Ptr>> mapBuffers(
    Device::Ptr device, BufType buf_type,
    const std::vector<std::vector<v4l2_plane>> &planes);

class MMapBuffers {
 public:
  MMapBuffers(CaptureDevice device, int num_bufs)
      : buf_type_(device.GetBufType()),
        num_planes_(getNumPlanes(device.GetDevice()->fd(), buf_type_)),
        planes_(allocatePlanes(device.GetDevice(), buf_type_, num_bufs)),
        buffers_(mapBuffers(device.GetDevice(), buf_type_, planes_)),
        device_(device.GetDevice()) {}

  v4l2_buffer PrepareV4L2Buffer(int idx) {
    v4l2_buffer v4l2_buffer;
    memset(&v4l2_buffer, 0, sizeof(v4l2_buffer));
    v4l2_buffer.type = buf_type_;
    v4l2_buffer.memory = V4L2_MEMORY_MMAP;
    v4l2_buffer.index = idx;
    if (device_->GetCapabilities().IsMPlane()) {
      v4l2_buffer.m.planes = planes_[idx].data();
      v4l2_buffer.length = planes_[idx].size();
    }
    return v4l2_buffer;
  }
  std::vector<Buffer::Ptr> GetBuffers(int idx) { return buffers_[idx]; }
  std::vector<v4l2_plane> GetPlanes(int idx) { return planes_[idx]; }
  uint32_t NumPlanes() const { return num_planes_; }
  int NumBuffers() const { return buffers_.size(); }

 private:
  BufType buf_type_;
  uint32_t num_planes_;
  std::vector<std::vector<v4l2_plane>> planes_;
  std::vector<std::vector<Buffer::Ptr>> buffers_;
  std::shared_ptr<Device> device_;
};

}  // namespace internal

class MMapFrame : public Frame {
  MMapStream::Ptr stream_;
  std::vector<internal::Buffer::Ptr> buffers_;
  std::vector<uint32_t> bytes_used_;
  int buffer_idx_;
  uint64_t seq_id_;
  time_point frame_time_;

 public:
  MMapFrame(MMapStream::Ptr stream, std::vector<internal::Buffer::Ptr> buffers,
            std::vector<uint32_t> bytes_used, uint64_t seq_id,
            time_point frame_time, int buffer_idx)
      : stream_(stream),
        buffers_(buffers),
        bytes_used_(bytes_used),
        buffer_idx_(buffer_idx),
        seq_id_(seq_id),
        frame_time_(frame_time) {}
  ~MMapFrame() override { stream_->QueueBuffer(buffer_idx_); }
  void Process(uint32_t plane,
               std::function<void(uint8_t *, uint64_t)> fn) const override {
    fn(static_cast<uint8_t *>(buffers_[plane]->start), bytes_used_[plane]);
  }

  uint64_t SeqId() const override { return seq_id_; }

  time_point Time() const override { return frame_time_; }

  uint32_t NumPlanes() const override { return buffers_.size(); }
};

void MMapStream::DoStart() {
  spdlog::info("starting stream");
  buffers = std::make_unique<internal::MMapBuffers>(device_, 4);
  for (int i = 0; i < buffers->NumBuffers(); ++i) {
    QueueBuffer(i);
  }
  int bufType = device_.GetBufType();
  int ret = ioctl(device_.GetDevice()->fd(), VIDIOC_STREAMON, &bufType);
  if (ret < 0)
    throw Exception(fmt::format("Failed to start stream: {}", strerror(errno)));
}
void MMapStream::DoStop() {
  spdlog::info("stopping stream");
  int bufType = device_.GetBufType();
  int ret = ioctl(device_.GetDevice()->fd(), VIDIOC_STREAMOFF, &bufType);
  if (ret < 0)
    throw Exception(fmt::format("Failed to stop stream: {}", strerror(errno)));
  requestBuffers(device_.GetDevice()->fd(), device_.GetBufType(), 0);
  // todo lock
  buffers.reset();
}

MMapStream::MMapStream(CaptureDevice device) : Stream<MMapStream>(device) {}
void MMapStream::QueueBuffer(int idx) {
  v4l2_buffer buffer = buffers->PrepareV4L2Buffer(idx);
  int ret = ioctl(device_.GetDevice()->fd(), VIDIOC_QBUF, &buffer);
  if (ret < 0) {
    throw Exception(fmt::format("Failed to q buf {}", strerror(errno)));
  }
}

Frame::Ptr MMapStream::FetchNext() {
  v4l2_buffer buffer;
  memset(&buffer, 0, sizeof(v4l2_buffer));
  buffer.type = device_.GetBufType();
  buffer.memory = V4L2_MEMORY_MMAP;
  std::vector<v4l2_plane> planes(buffers->NumPlanes());
  for (uint32_t i = 0; i < buffers->NumPlanes(); ++i) {
    memset(&planes[i], 0, sizeof(v4l2_plane));
  }
  if (device_.GetDevice()->GetCapabilities().IsMPlane()) {
    buffer.m.planes = planes.data();
    buffer.length = planes.size();
  }
  int ret = ioctl(device_.GetDevice()->fd(), VIDIOC_DQBUF, &buffer);
  if (ret < 0) {
    throw Exception(fmt::format("Failed to dq buf: {}", strerror(errno)));
  }
  if (buffer.flags & V4L2_BUF_FLAG_LAST) {
    spdlog::info("last buf");
  }
  buffer.flags &= ~V4L2_BUF_FLAG_LAST;

  auto next = buffer.index;
  std::vector<uint32_t> bytes_used;
  if (device_.GetDevice()->GetCapabilities().IsMPlane()) {
    for (const auto &plane : buffers->GetPlanes(next)) {
      bytes_used.push_back(plane.bytesused);
    }
  } else {
    bytes_used.push_back(buffer.bytesused);
  }
  const std::chrono::system_clock::duration micros = std::chrono::microseconds(
      uint64_t(buffer.timestamp.tv_sec * 1e6 + buffer.timestamp.tv_usec));
  Frame::time_point frame_time(micros);

  auto this_ptr = shared_from_this();

  return std::make_shared<MMapFrame>(this_ptr, buffers->GetBuffers(next),
                                     bytes_used, buffer.sequence, frame_time,
                                     next);
}
MMapStream::~MMapStream() {}

v4s::internal::Buffer::Buffer(int fd, int offset, size_t length)
    : start(allocateBuffer(fd, offset, length)), length(length) {}

v4s::internal::Buffer::~Buffer() { munmap(start, length); }

std::vector<std::vector<internal::Buffer::Ptr>> internal::mapBuffers(
    Device::Ptr device, BufType buf_type,
    const std::vector<std::vector<v4l2_plane>> &planes) {
  bool mplane = device->GetCapabilities().IsMPlane();
  std::vector<std::vector<internal::Buffer::Ptr>> buffers;
  for (int bufIdx = 0; bufIdx < planes.size(); ++bufIdx) {
    std::vector<internal::Buffer::Ptr> plane_buffers;
    std::vector<v4l2_plane> buf_planes = planes[bufIdx];
    v4l2_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    buffer.type = buf_type;
    buffer.memory = V4L2_MEMORY_MMAP;
    buffer.index = bufIdx;
    if (mplane) {
      buffer.m.planes = buf_planes.data();
      buffer.length = buf_planes.size();
    }
    int ret = ioctl(device->fd(), VIDIOC_QUERYBUF, &buffer);
    if (ret < 0) {
      throw Exception(fmt::format("failed to query buf: {}", strerror(errno)));
    }
    if (mplane) {
      for (int planeIdx = 0; planeIdx < buf_planes.size(); ++planeIdx) {
        plane_buffers.push_back(std::make_unique<internal::Buffer>(
            device->fd(), buf_planes[planeIdx].m.mem_offset,
            buf_planes[planeIdx].length));
      }
    } else {
      plane_buffers.push_back(std::make_unique<internal::Buffer>(
          device->fd(), buffer.m.offset, buffer.length));
    }
    buffers.push_back(plane_buffers);
  }
  return buffers;
}

}  // namespace v4s
