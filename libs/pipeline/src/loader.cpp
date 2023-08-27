#include "pipeline/loader.h"

#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "pipeline/config.h"
#include "pipeline_impl.h"
#include "v4l/v4l_capture.h"
#include "v4l/v4l_output.h"

namespace v4s {
std::unordered_map<std::string, Device::Ptr> LoadDevices(
    const PipelineConfig &config) {
  std::unordered_map<std::string, Device::Ptr> devices;
  for (auto bridge : config.bridges) {
    if (devices.find(bridge.sink) == devices.end())
      devices[bridge.sink] = Device::from_devnode(bridge.sink);
    if (devices.find(bridge.source) == devices.end())
      devices[bridge.source] = Device::from_devnode(bridge.source);
  }
  if (devices.find(config.source.source) == devices.end())
    devices[config.source.source] = Device::from_devnode(config.source.source);
  return devices;
}
PipelineLoader::PipelineLoader(PipelineConfig config)
    : config_(config), devices_(LoadDevices(config_)) {}

Pipeline PipelineLoader::Load() const {
  std::vector<Bridge::Ptr> bridges;
  if (devices_.empty()) {
    throw Exception("No devices to load");
  }
  for (const auto &bridge_config : config_.bridges) {
    auto it = devices_.find(bridge_config.source);
    std::optional<CaptureDevice> capture_device = *it->second->TryCapture();
    it = devices_.find(bridge_config.sink);
    std::optional<OutputDevice> output_device = *it->second->TryOutput();
    if (!capture_device || !output_device) {
      throw Exception("Bridge needs a capture device and an output device");
    }
    bridges.emplace_back(std::make_shared<Bridge>(capture_device.value(),
                                                  output_device.value()));
  }
  auto cap_device = devices_.find(config_.source.source)->second->TryCapture();
  if (!cap_device) {
    throw Exception("Pipeline needs a capture device");
  }
  v4s::Pipeline pipeline(std::make_shared<internal::PipelineImpl>(
      std::make_shared<MMapStream>(cap_device.value()), bridges));
  return pipeline;
}
}  // namespace v4s
