#include "pipeline/loader.h"

#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "algorithm.h"
#include "decoder.h"
#include "feedback.h"
#include "pipeline/config.h"
#include "pipeline_impl.h"
#include "v4l/v4l_capture.h"
#include "v4l/v4l_meta_cap.h"
#include "v4l/v4l_output.h"
#include "v4l/v4l_stream.h"

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
  for (auto cc : config.controls) {
    if (devices.find(cc.stats_device.source) == devices.end())
      devices[cc.stats_device.source] =
          Device::from_devnode(cc.stats_device.source);
  }
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
  std::vector<PipelineControl::Ptr> controls;
  for (const auto &cc : config_.controls) {
    auto it = devices_.find(cc.stats_device.source);
    std::optional<MetaCaptureDevice> meta_cap_device =
        *it->second->TryMetaCapture();
    if (!meta_cap_device) {
      throw Exception("Meta source needs a capture device");
    }
    std::vector<Algorithm::Ptr> algorithms;
    for (const auto &[name, ctrl_device_name] : cc.algorithms) {
      auto cit = devices_.find(ctrl_device_name);
      if (cit == devices_.end()) {
        throw Exception("Algorithm needs a control device");
      }
      auto algorithm = GetAlgorithm(name, cit->second);
      if (!algorithm) {
        throw Exception("Unknown algorithm");
      }
      algorithms.emplace_back(algorithm);
    }
    auto decoder = GetDecoder(cc.stats_device.decoder);
    if (!decoder) {
      throw Exception("Unknown decoder");
    }

    controls.emplace_back(std::make_shared<PipelineControl>(
        StatsSource{
            .stream = std::make_shared<MMapStream>(meta_cap_device.value()),
            .decoder = decoder.value(),
            .codec = cc.stats_device.codec,
        },
        algorithms));
  }
  v4s::Pipeline pipeline(std::make_shared<internal::PipelineImpl>(
      std::make_shared<MMapStream>(cap_device.value()), bridges, controls));
  return pipeline;
}
}  // namespace v4s
