#ifndef V4L2_STREAM_V4L_CAPTURE_H
#define V4L2_STREAM_V4L_CAPTURE_H
#include "v4l/v4l_device.h"
#include "v4l/v4l_framesize.h"

namespace v4s {
class CaptureDevice {
public:
  CaptureDevice(std::shared_ptr<Device> device);

  Format GetFormat() const;

  Format SetFormat(Format format);

  BufType GetBufType() const;

  Device::Ptr GetDevice() const;

private:
  Device::Ptr device_;
};
} // namespace v4s

#endif // !V4L2_STREAM_V4L_CAPTURE_H
