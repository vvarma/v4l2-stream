#ifndef V4L2_STREAM_DEVICE_H
#define V4L2_STREAM_DEVICE_H

#include <chrono>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "v4l_caps.h"

namespace v4s {
class CaptureDevice;
class OutputDevice;

class Device : public std::enable_shared_from_this<Device> {
  int fd_;
  Capabilities capabilities_;

public:
  typedef std::shared_ptr<Device> Ptr;

  Device(int fd, Capabilities Capabilities);
  ~Device();

  int fd() const;

  static Device::Ptr from_devnode(const std::string &path);

  Capabilities GetCapabilities() const;
  std::optional<CaptureDevice> TryCapture();
};

} // namespace v4s

#endif // !V4L2_STREAM_DEVICE_H
