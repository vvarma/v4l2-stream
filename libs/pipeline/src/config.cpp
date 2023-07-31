#include "pipeline/config.h"

#include <fstream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;
using namespace nlohmann::literals;

namespace v4s {
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PipelineConfig, devices)

PipelineConfig PipelineConfig::FromFile(const std::string &path) {
  std::ifstream f(path);
  return nlohmann::json::parse(f).get<PipelineConfig>();
}
} // namespace v4s
