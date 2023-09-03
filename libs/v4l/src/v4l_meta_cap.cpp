#include "v4l/v4l_meta_cap.h"

#include <cassert>

namespace v4s {
MetaCaptureDevice::MetaCaptureDevice(std::shared_ptr<Device> device)
    : device_(device) {}

BufType MetaCaptureDevice::GetBufType() const {
  auto capabilities = device_->GetCapabilities();
  assert((capabilities.caps & std::bitset<32>(CAP_META_CAPTURE)).any() &&
         "device doesnt support meta capture");
  return BUF_META_CAPTURE;
}
Device::Ptr MetaCaptureDevice::GetDevice() const { return device_; }
}  // namespace v4s
