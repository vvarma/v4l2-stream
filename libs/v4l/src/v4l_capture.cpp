#include "v4l/v4l_capture.h"

#include "v4l/v4l_device.h"
#include "v4l/v4l_framesize.h"
#include "v4l/v4l_stream.h"

#include "util.h"

namespace v4s {
CaptureDevice::CaptureDevice(Device::Ptr device) : device_(device) {}

Format CaptureDevice::GetFormat() const {
  return getFormat(device_, GetBufType());
}
Format CaptureDevice::SetFormat(Format format) {
  return setFormat(device_, GetBufType(), format);
}

Device::Ptr CaptureDevice::Device() const { return device_; }

BufType CaptureDevice::GetBufType() const {
  auto capabilities = device_->GetCapabilities();

  if (capabilities.IsMPlane()) {
    return BUF_VIDEO_CAPTURE_MPLANE;
  }
  return BUF_VIDEO_CAPTURE;
}

} // namespace v4s
