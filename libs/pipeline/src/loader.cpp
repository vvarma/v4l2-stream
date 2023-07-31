#include "pipeline/loader.h"
#include "pipeline/config.h"
#include <vector>

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

} // namespace v4s
