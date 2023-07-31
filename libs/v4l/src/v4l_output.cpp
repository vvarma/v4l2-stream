#include "v4l/v4l_output.h"

#include "util.h"
#include "v4l/v4l_device.h"
#include "v4l/v4l_framesize.h"
namespace v4s {
OutputDevice::OutputDevice(Device::Ptr device) : device_(device) {}

Format OutputDevice::GetFormat() const {
  return getFormat(device_, GetBufType());
}
Format OutputDevice::SetFormat(Format format) {
  return setFormat(device_, GetBufType(), format);
}
Device::Ptr OutputDevice::GetDevice() const { return device_; }

BufType OutputDevice::GetBufType() const {
  auto capabilities = device_->GetCapabilities();

  if (capabilities.IsMPlane()) {
    return BUF_VIDEO_OUTPUT_MPLANE;
  }
  return BUF_VIDEO_OUTPUT;
}

} // namespace v4s
