#ifndef V4L2_STREAM_DEVICE_H
#define V4L2_STREAM_DEVICE_H

#include <chrono>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "v4l/v4l_controls.h"
#include "v4l_caps.h"

namespace v4s {
enum BufType {
  BUF_VIDEO_CAPTURE = 1,
  BUF_VIDEO_OUTPUT = 2,
  BUF_VIDEO_CAPTURE_MPLANE = 9,
  BUF_VIDEO_OUTPUT_MPLANE = 10
};
class CaptureDevice;
class OutputDevice;

class Device : public std::enable_shared_from_this<Device> {
  int fd_;
  std::string devnode_;
  Capabilities capabilities_;
  std::vector<Control::Ptr> controls_;

 public:
  typedef std::shared_ptr<Device> Ptr;

  Device(const std::string &devnode, int fd, Capabilities Capabilities);
  ~Device();

  int fd() const;
  std::string_view DevNode() const;

  static Device::Ptr from_devnode(const std::string &path);

  Capabilities GetCapabilities() const;
  std::optional<CaptureDevice> TryCapture();
  std::optional<OutputDevice> TryOutput();

  std::vector<Control::Ptr> GetControls();
  void SetControl(uint32_t id, int64_t val);
};

}  // namespace v4s

#endif  // !V4L2_STREAM_DEVICE_H
