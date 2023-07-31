#ifndef V4L2_STREAM_V4L_OUTPUT_H
#define V4L2_STREAM_V4L_OUTPUT_H
#include <memory>

#include "v4l/v4l_device.h"
#include "v4l/v4l_framesize.h"
namespace v4s {
class OutputDevice {
public:
  OutputDevice(std::shared_ptr<Device> device);
  Format GetFormat() const;
  Format SetFormat(Format format);
  Device::Ptr GetDevice() const;
  BufType GetBufType() const;

private:
  Device::Ptr device_;
};

} // namespace v4s

#endif // V4L2_STREAM_V4L_OUTPUT_H
