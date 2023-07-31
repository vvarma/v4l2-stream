#include "util.h"

#include <cstdint>
#include <linux/videodev2.h>
#include <sys/ioctl.h>

#include <fmt/format.h>
#include <sys/mman.h>

#include "v4l/v4l_codec.h"
#include "v4l/v4l_device.h"
#include "v4l/v4l_exception.h"
#include "v4l/v4l_framesize.h"

uint16_t requestBuffers(int fd, v4s::BufType bufType, uint32_t num_bufs) {
  v4l2_requestbuffers requestbuffers;
  memset(&requestbuffers, 0, sizeof(v4l2_requestbuffers));
  requestbuffers.type = bufType;
  requestbuffers.count = 4;
  requestbuffers.memory = V4L2_MEMORY_MMAP;
  int ret = ioctl(fd, VIDIOC_REQBUFS, &requestbuffers);
  if (ret < 0)
    throw v4s::Exception(
        fmt::format("Failed to request buffers: {}", strerror(errno)));
  return requestbuffers.count;
}

uint32_t getNumPlanes(int fd, v4s::BufType bufType) {
  if (bufType == v4s::BUF_VIDEO_CAPTURE || bufType == v4s::BUF_VIDEO_OUTPUT)
    return 1;
  v4l2_format fmt;
  memset(&fmt, 0, sizeof(v4l2_format));
  fmt.type = bufType;
  int ret = ioctl(fd, VIDIOC_G_FMT, &fmt);
  if (ret < 0)
    throw v4s::Exception(
        fmt::format("Failed to get format: {}", strerror(errno)));
  return fmt.fmt.pix_mp.num_planes;
}

std::vector<std::vector<v4l2_plane>>
allocatePlanes(v4s::Device::Ptr device, v4s::BufType buf_type, int num_bufs) {
  int num_buffers = requestBuffers(device->fd(), buf_type, 4);
  uint32_t num_planes = getNumPlanes(device->fd(), buf_type);
  std::vector<std::vector<v4l2_plane>> planes;
  for (int i = 0; i < num_buffers; ++i) {
    std::vector<v4l2_plane> buf_planes;
    for (int j = 0; j < num_planes; ++j) {
      v4l2_plane plane;
      memset(&plane, 0, sizeof(v4l2_plane));
      buf_planes.push_back(plane);
    }
    planes.push_back(buf_planes);
  }
  return planes;
}

void *allocateBuffer(int fd, int offset, size_t length) {
  return mmap(nullptr, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset);
}

v4s::Codec FromFourcc(uint32_t fourcc) {
  switch (fourcc) {
  case V4L2_PIX_FMT_MJPEG:
    return v4s::Codec::MJPG;
  case V4L2_PIX_FMT_YUYV:
    return v4s::Codec::YUYV;
  case V4L2_PIX_FMT_SRGGB8:
    return v4s::Codec::RGGB;
  default:
    throw v4s::Exception("Unsupported format");
  }
}

uint32_t ToFourcc(v4s::Codec codec) {
  switch (codec) {
  case v4s::Codec::MJPG:
    return V4L2_PIX_FMT_MJPEG;
  case v4s::Codec::YUYV:
    return V4L2_PIX_FMT_YUYV;
  case v4s::Codec::RGGB:
    return V4L2_PIX_FMT_SRGGB8;
  default:
    throw v4s::Exception("Unsupported format");
  }
}

v4s::Format getFormat(v4s::Device::Ptr device, v4s::BufType buf_type) {
  v4l2_format fmt;
  memset(&fmt, 0, sizeof(fmt));
  fmt.type = buf_type;
  int ret = ioctl(device->fd(), VIDIOC_G_FMT, &fmt);
  if (ret < 0) {
    throw v4s::Exception("Failed to get format");
  }
  return v4s::Format{FromFourcc(fmt.fmt.pix.pixelformat), fmt.fmt.pix.width,
                     fmt.fmt.pix.height};
}

v4s::Format setFormat(v4s::Device::Ptr device, v4s::BufType buf_type,
                      v4s::Format format) {
  v4l2_format fmt;
  memset(&fmt, 0, sizeof(fmt));
  fmt.type = buf_type;
  fmt.fmt.pix.width = format.width;
  fmt.fmt.pix.height = format.height;
  fmt.fmt.pix.pixelformat = ToFourcc(format.codec);
  int ret = ioctl(device->fd(), VIDIOC_S_FMT, &fmt);
  if (ret < 0) {
    throw v4s::Exception("Failed to set format");
  }
  return v4s::Format{FromFourcc(fmt.fmt.pix.pixelformat), fmt.fmt.pix.width,
                     fmt.fmt.pix.height};
}
