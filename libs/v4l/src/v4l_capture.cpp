#include "v4l/v4l_capture.h"
#include "v4l/v4l_device.h"
#include "v4l/v4l_stream.h"

namespace v4s {
CaptureDevice::CaptureDevice(Device::Ptr device) : device_(device) {}

MMapStream::Ptr CaptureDevice::Stream() {
  return std::make_shared<MMapStream>(device_);
}

} // namespace v4s
