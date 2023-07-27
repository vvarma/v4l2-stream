#include "v4l/v4l_device.h"

#include <bitset>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <fmt/core.h>
#include <memory>
#include <optional>
#include <unistd.h>

#include <linux/videodev2.h>
#include <sys/ioctl.h>

#include <fmt/format.h>

#include "v4l/v4l_caps.h"
#include "v4l/v4l_capture.h"
#include "v4l/v4l_exception.h"

namespace v4s {
Capabilities getCapabilities(int fd) {
  v4l2_capability cap;
  memset(&cap, 0, sizeof(cap));
  int ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
  if (ret < 0)
    throw Exception(fmt::format("Error query device caps {}", strerror(errno)));
  return {
      .driver = reinterpret_cast<char *>(cap.driver),
      .card = reinterpret_cast<char *>(cap.card),
      .bus_info = reinterpret_cast<char *>(cap.bus_info),
      .kernel_version = {static_cast<uint8_t>(cap.version >> 24 & 0xff),
                         static_cast<uint8_t>(cap.version >> 16 & 0xff),
                         static_cast<uint8_t>(cap.version >> 8 & 0xff)},
      .caps = cap.capabilities,
  };
}

Device::Device(int fd, Capabilities capabilities)
    : fd_(fd), capabilities_(capabilities) {}

Device::Ptr Device::from_devnode(const std::string &path) {
  int fd = open(path.c_str(), O_RDWR);
  if (fd < 0) {
    throw Exception(
        fmt::format("Error opening device: {} err: {}", path, strerror(errno)));
  }

  return std::make_shared<Device>(fd, getCapabilities(fd));
}

Device::~Device() { close(fd_); }

BufType Device::GetBufType(bool capture) const {
  if (capture) {
    if (!capabilities_.IsCapture()) {
      throw Exception("Device does not support capture");
    }
    if (capabilities_.IsMPlane()) {
      return BUF_VIDEO_CAPTURE_MPLANE;
    }
    return BUF_VIDEO_CAPTURE;
  } else {
    if (!capabilities_.IsOutput()) {
      throw Exception("Device does not support ouput");
    }
    if (capabilities_.IsMPlane()) {
      return BUF_VIDEO_OUTPUT_MPLANE;
    }
    return BUF_VIDEO_OUTPUT;
  }
}

int Device::fd() const { return fd_; }

Capabilities Device::GetCapabilities() const { return capabilities_; }

std::optional<CaptureDevice> Device::TryCapture() {
  Capabilities caps = GetCapabilities();
  if (!(caps.caps & kVideoCaptureDevice).any()) {
    return std::nullopt;
  }
  return CaptureDevice(shared_from_this());
}

} // namespace v4s
