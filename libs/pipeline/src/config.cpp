#include "pipeline/config.h"

#include <fstream>
#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using namespace nlohmann::literals;

namespace v4s {
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BridgeConfig, source, sink);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SourceConfig, source);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ControlConfig, stats_device, decoder_type,
                                   ctrl_device);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PipelineConfig, bridges, source, controls)

PipelineConfig PipelineConfig::FromFile(const std::string &path) {
  std::ifstream f(path);
  return nlohmann::json::parse(f).get<PipelineConfig>();
}
}  // namespace v4s
