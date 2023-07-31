#include "v4l/v4l_bridge.h"

#include <cstring>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <vector>

#include <spdlog/spdlog.h>

#include "v4l/v4l_device.h"
#include "v4l/v4l_exception.h"

#include "util.h"

namespace v4s {

void requestOutputDMABuffers(Device::Ptr device, int num_bufs);
void queueBuffer(Device::Ptr device, int idx);
namespace internal {

std::vector<std::vector<int>> mapBuffers(Device::Ptr device, int num_bufs,
                                         int num_planes);
class BridgeBuffers {
public:
  BridgeBuffers(Device::Ptr capture_device, int num_bufs);
  int NumBufs() const { return dma_fds_.size(); }
  int NumPlanes() const { return num_planes_; }

private:
  std::shared_ptr<Device> device_;
  BufType buf_type_;
  int num_bufs_, num_planes_;
  std::vector<std::vector<int>> dma_fds_;
};
} // namespace internal
Bridge::Bridge(CaptureDevice capture_device, OutputDevice output_device)
    : capture_device_(capture_device), output_device_(output_device) {}

void Bridge::Start() {
  auto output_device = output_device_.Device();
  auto capture_device = capture_device_.Device();
  if (getNumPlanes(capture_device->fd(), capture_device_.GetBufType()) !=
      getNumPlanes(output_device->fd(), output_device_.GetBufType())) {
    throw Exception(
        "Capture and output device must have the same number of planes");
  }

  buffers_ = std::make_unique<internal::BridgeBuffers>(capture_device, 4);
  requestOutputDMABuffers(output_device, buffers_->NumBufs());
  for (int i = 0; i < buffers_->NumBufs(); ++i) {
    queueBuffer(capture_device, i);
  }
  int capture_buf_type = capture_device_.GetBufType();
  int output_buf_type = output_device_.GetBufType();
  int ret = ioctl(capture_device->fd(), VIDIOC_STREAMON, &capture_buf_type);
  if (ret < 0) {
    throw Exception("Failed to start capture device");
  }
  ret = ioctl(output_device->fd(), VIDIOC_STREAMON, &output_buf_type);
  if (ret < 0) {
    throw Exception("Failed to start output device");
  }
  spdlog::debug("Stream started for {}->{}",
                capture_device->GetCapabilities().driver,
                output_device->GetCapabilities().driver);
}
void Bridge::Stop() {
  auto output_device = output_device_.Device();
  auto capture_device = capture_device_.Device();
  int capture_buf_type = capture_device_.GetBufType();
  int output_buf_type = output_device_.GetBufType();
  int ret = ioctl(capture_device->fd(), VIDIOC_STREAMOFF, &capture_buf_type);
  if (ret < 0) {
    throw Exception("Failed to stop capture device");
  }
  ret = ioctl(output_device->fd(), VIDIOC_STREAMOFF, &output_buf_type);
  if (ret < 0) {
    throw Exception("Failed to stop output device");
  }
  buffers_ = nullptr;
}
void Bridge::ProcessRead() {
  auto output_device = output_device_.Device();
  auto capture_device = capture_device_.Device();
  spdlog::debug("Process Read for {}->{}",
                capture_device->GetCapabilities().driver,
                output_device->GetCapabilities().driver);
  v4l2_buffer cap_buffer;
  memset(&cap_buffer, 0, sizeof(v4l2_buffer));
  cap_buffer.type = capture_device_.GetBufType();
  cap_buffer.memory = V4L2_MEMORY_MMAP;
  std::vector<v4l2_plane> cap_planes(buffers_->NumPlanes());
  for (auto &plane : cap_planes) {
    memset(&plane, 0, sizeof(plane));
  }
  bool cap_mplane = capture_device->GetCapabilities().IsMPlane();
  if (cap_mplane) {
    cap_buffer.m.planes = cap_planes.data();
    cap_buffer.length = buffers_->NumPlanes();
  }
  int ret = ioctl(capture_device->fd(), VIDIOC_DQBUF, &cap_buffer);
  if (ret < 0) {
    throw Exception("Failed to dequeue buffer");
  }
  std::vector<std::pair<int, int>> plane_info;
  if (cap_mplane) {
    for (const auto &p : cap_planes) {
      plane_info.emplace_back(p.m.fd, p.bytesused);
    }
  } else {
    plane_info.emplace_back(cap_buffer.m.fd, cap_buffer.bytesused);
  }

  v4l2_buffer buffer;
  memset(&buffer, 0, sizeof(v4l2_buffer));
  buffer.type = output_device_.GetBufType();
  buffer.memory = V4L2_MEMORY_DMABUF;
  std::vector<v4l2_plane> out_planes(plane_info.size());
  if (output_device->GetCapabilities().IsMPlane()) {
    for (size_t i = 0; i < out_planes.size(); ++i) {
      out_planes[i].m.fd = plane_info[i].first;
      out_planes[i].length = plane_info[i].second;
    }
    buffer.m.planes = out_planes.data();
    buffer.length = out_planes.size();
  } else {
    buffer.bytesused = plane_info[0].second;
    buffer.m.fd = plane_info[0].first;
  }
  buffer.timestamp = cap_buffer.timestamp;
  buffer.flags |= V4L2_BUF_FLAG_TIMESTAMP_COPY;
  ret = ioctl(output_device->fd(), VIDIOC_QBUF, &buffer);
  if (ret < 0) {
    throw Exception("Failed to queue buffer");
  }
}

void Bridge::ProcessWrite() {
  auto output_device = output_device_.Device();
  auto capture_device = capture_device_.Device();
  spdlog::debug("Process Write for {}->{}",
                capture_device->GetCapabilities().driver,
                output_device->GetCapabilities().driver);
  // dequeue output buffer and enqueue capture buffer
  int bufIdx = -1;
  {
    v4l2_buffer buffer;
    memset(&buffer, 0, sizeof(v4l2_buffer));
    buffer.type = output_device_.GetBufType();
    buffer.memory = V4L2_MEMORY_DMABUF;
    std::vector<v4l2_plane> planes(buffers_->NumPlanes());
    for (auto &plane : planes) {
      memset(&plane, 0, sizeof(plane));
    }
    if (output_device->GetCapabilities().IsMPlane()) {
      buffer.m.planes = planes.data();
      buffer.length = buffers_->NumPlanes();
    }
    int ret = ioctl(output_device->fd(), VIDIOC_DQBUF, &buffer);
    if (ret < 0) {
      throw Exception("Failed to dequeue buffer");
    }
    bufIdx = buffer.index;
  }
  v4l2_buffer buffer;
  memset(&buffer, 0, sizeof(v4l2_buffer));
  buffer.type = capture_device->GetBufType();
  buffer.memory = V4L2_MEMORY_MMAP;
  std::vector<v4l2_plane> planes(buffers_->NumPlanes());
  for (auto &plane : planes) {
    memset(&plane, 0, sizeof(plane));
  }
  if (capture_device->GetCapabilities().IsMPlane()) {
    buffer.m.planes = planes.data();
    buffer.length = buffers_->NumPlanes();
  }
  buffer.index = bufIdx;
  int ret = ioctl(capture_device->fd(), VIDIOC_QBUF, &buffer);
  if (ret < 0) {
    throw Exception("Failed to enqueue buffer");
  }
}

int Bridge::ReadFd() const { return capture_device_.Device()->fd(); }
int Bridge::WriteFd() const { return output_device_.Device()->fd(); }
CaptureDevice Bridge::GetCaptureDevice() const { return capture_device_; }
OutputDevice Bridge::GetOutputDevice() const { return output_device_; }

void Bridge::QueueBuffer(int idx) {
  auto capture_device = capture_device_.Device();
  v4l2_buffer buf;
  memset(&buf, 0, sizeof(v4l2_buffer));
  BufType buf_type = capture_device_.GetBufType();
  buf.type = buf_type;
  buf.memory = V4L2_MEMORY_MMAP;
  buf.index = idx;
  std::vector<v4l2_plane> planes(buffers_->NumPlanes());
  for (auto &plane : planes) {
    memset(&plane, 0, sizeof(plane));
  }
  if (capture_device->GetCapabilities().IsMPlane()) {
    buf.m.planes = planes.data();
    buf.length = buffers_->NumPlanes();
  }
  int ret = ioctl(capture_device->fd(), VIDIOC_QBUF, &buf);
  if (ret < 0) {
    throw Exception("Failed to queue buffer");
  }
}
Bridge::~Bridge() {}

internal::BridgeBuffers::BridgeBuffers(Device::Ptr capture_device, int num_bufs)
    : device_(capture_device), buf_type_(device_->GetBufType()),
      num_bufs_(requestBuffers(device_->fd(), buf_type_, num_bufs)),
      num_planes_(getNumPlanes(device_->fd(), buf_type_)),
      dma_fds_(mapBuffers(device_, num_bufs_, num_planes_)) {}

std::vector<std::vector<int>>
internal::mapBuffers(Device::Ptr device, int num_bufs, int num_planes) {
  bool mplane = device->GetCapabilities().IsMPlane();
  BufType bufType = device->GetBufType();
  std::vector<std::vector<int>> buffers;
  for (int bufIdx = 0; bufIdx < num_bufs; ++bufIdx) {
    std::vector<int> plane_buffers;
    if (!mplane) {
      v4l2_exportbuffer expbuf;
      memset(&expbuf, 0, sizeof(v4l2_exportbuffer));
      expbuf.type = bufType;
      expbuf.index = bufIdx;
      int ret = ioctl(device->fd(), VIDIOC_EXPBUF, &expbuf);
      if (ret < 0) {
        throw Exception("Failed to export buffer");
      }
      plane_buffers.push_back(expbuf.fd);
    } else {
      for (int planeIdx = 0; planeIdx < num_planes; ++planeIdx) {
        v4l2_exportbuffer expbuf;
        memset(&expbuf, 0, sizeof(v4l2_exportbuffer));
        expbuf.type = bufType;
        expbuf.index = bufIdx;
        expbuf.plane = planeIdx;
        int ret = ioctl(device->fd(), VIDIOC_EXPBUF, &expbuf);
        if (ret < 0) {
          throw Exception("Failed to export buffer");
        }
        plane_buffers.push_back(expbuf.fd);
      }
    }
    buffers.push_back(plane_buffers);
  }
  return buffers;
}
void requestOutputDMABuffers(Device::Ptr device, int num_bufs) {
  v4l2_requestbuffers reqbuf;
  memset(&reqbuf, 0, sizeof(v4l2_requestbuffers));
  BufType out_buf_type = device->GetBufType(false);
  reqbuf.type = out_buf_type;
  reqbuf.memory = V4L2_MEMORY_DMABUF;
  reqbuf.count = num_bufs;
  int ret = ioctl(device->fd(), VIDIOC_REQBUFS, &reqbuf);
  if (ret < 0) {
    throw Exception("Failed to request buffers");
  }
  if (reqbuf.count != num_bufs) {
    throw Exception("Failed to allocate requested number of buffers");
  }
}
void queueBuffer(Device::Ptr device, int idx) {}
} // namespace v4s
