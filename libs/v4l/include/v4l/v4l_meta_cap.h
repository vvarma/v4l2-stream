#pragma once
#include "v4l/v4l_device.h"
#include "v4l/v4l_framesize.h"
namespace v4s {
class MetaCaptureDevice {
 public:
  MetaCaptureDevice(std::shared_ptr<Device> device);
  BufType GetBufType() const;
  Device::Ptr GetDevice() const;

 private:
  Device::Ptr device_;
};
}  // namespace v4s
