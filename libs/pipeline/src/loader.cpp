#include "pipeline/loader.h"

#include <memory>
#include <vector>

#include "pipeline/config.h"
#include "pipeline_impl.h"

namespace v4s {
std::vector<Device::Ptr> LoadDevices(PipelineConfig config) {
  std::vector<Device::Ptr> devices;
  for (auto device_name : config.devices) {
    devices.push_back(Device::from_devnode(device_name));
  }
  return devices;
}
PipelineLoader::PipelineLoader(PipelineConfig config)
    : devices_(LoadDevices(config)) {}

Pipeline PipelineLoader::Load() const {
  std::vector<Bridge::Ptr> bridges;
  if (devices_.empty()) {
    throw Exception("No devices to load");
  }
  if (devices_.size() == 1) {
    auto cap_device = devices_[0]->TryCapture();
    if (!cap_device) {
      throw Exception("Pipeline needs a capture device");
    }
    auto sink = std::make_shared<MMapStream>(cap_device.value());
    return v4s::Pipeline(
        std::make_shared<internal::PipelineImpl>(sink, bridges));
  }
  for (size_t i = 0; i < devices_.size() - 1; i++) {
    auto capture_device = devices_[i]->TryCapture();
    auto output_device = devices_[i + 1]->TryOutput();
    if (!capture_device || !output_device) {
      throw Exception("Bridge needs a capture device and an output device");
    }
    bridges.emplace_back(std::make_shared<Bridge>(capture_device.value(),
                                                  output_device.value()));
  }
  auto cap_device = devices_[devices_.size() - 1]->TryCapture();
  if (!cap_device) {
    throw Exception("Pipeline needs a capture device");
  }
  v4s::Pipeline pipeline(std::make_shared<internal::PipelineImpl>(
      std::make_shared<MMapStream>(cap_device.value()), bridges));
  return pipeline;
}
}  // namespace v4s
