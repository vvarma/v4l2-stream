#include "v4l/v4l_device.h"

#include <asm-generic/errno-base.h>
#include <fcntl.h>
#include <fmt/format.h>
#include <linux/videodev2.h>
#include <spdlog/spdlog.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <bitset>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <memory>
#include <optional>

#include "v4l/v4l_caps.h"
#include "v4l/v4l_capture.h"
#include "v4l/v4l_controls.h"
#include "v4l/v4l_exception.h"
#include "v4l/v4l_output.h"

std::vector<v4s::Control::Ptr> QueryControls(int fd) {
  std::vector<v4s::Control::Ptr> controls;

  v4l2_query_ext_ctrl queryctrl;
  uint32_t start_id = V4L2_CID_BASE;

  while (true) {
    start_id |= V4L2_CTRL_FLAG_NEXT_CTRL;

    memset(&queryctrl, 0, sizeof(queryctrl));
    queryctrl.id = start_id;
    int ret = ioctl(fd, VIDIOC_QUERY_EXT_CTRL, &queryctrl);
    if (ret < 0) {
      break;
    }
    switch (queryctrl.type) {
      case V4L2_CTRL_TYPE_INTEGER:
        controls.push_back(std::make_shared<v4s::IntControl>(
            queryctrl.id, queryctrl.name, queryctrl.minimum, queryctrl.maximum,
            queryctrl.default_value, 0, queryctrl.step));
        break;
      default:
        spdlog::info("Unk Control: {} type: {}", queryctrl.name,
                     queryctrl.type);
    };
    start_id = queryctrl.id;
  }
  return controls;
}

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

Device::Device(const std::string &devnode, int fd, Capabilities capabilities)
    : fd_(fd),
      devnode_(devnode),
      capabilities_(capabilities),
      controls_(QueryControls(fd_)) {}

Device::Ptr Device::from_devnode(const std::string &path) {
  int fd = open(path.c_str(), O_RDWR);
  if (fd < 0) {
    throw Exception(
        fmt::format("Error opening device: {} err: {}", path, strerror(errno)));
  }

  return std::make_shared<Device>(path, fd, getCapabilities(fd));
}

Device::~Device() { close(fd_); }

int Device::fd() const { return fd_; }
std::string_view Device::DevNode() const { return devnode_; }

Capabilities Device::GetCapabilities() const { return capabilities_; }

std::optional<CaptureDevice> Device::TryCapture() {
  Capabilities caps = GetCapabilities();
  if (!(caps.caps & kVideoCaptureDevice).any()) {
    return std::nullopt;
  }
  return CaptureDevice(shared_from_this());
}

std::optional<OutputDevice> Device::TryOutput() {
  Capabilities caps = GetCapabilities();
  if (!(caps.caps & kVideoOutputDevice).any()) {
    return std::nullopt;
  }
  return OutputDevice(shared_from_this());
}

std::vector<Control::Ptr> Device::GetControls() {
  for (auto &control : controls_) {
    control->UpdateCurrent(fd_);
  }
  return controls_;
}

void Device::SetControl(uint32_t id, int64_t val) {
  for (auto &control : controls_) {
    if (control->id == id) {
      control->SetControl(fd_, val);
      return;
    }
  }
  throw Exception(fmt::format("Control {} not found", id));
}

}  // namespace v4s
