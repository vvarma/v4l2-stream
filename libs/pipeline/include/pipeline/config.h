#pragma once
#include <string>
#include <vector>
namespace v4s {

struct BridgeConfig {
  std::string source, sink;
};

struct SourceConfig {
  std::string source;
};

struct ControlConfig {
  std::string stats_device, decoder_type, ctrl_device;
};

struct PipelineConfig {
  std::vector<BridgeConfig> bridges;
  SourceConfig source;
  std::vector<ControlConfig> controls;

  static PipelineConfig FromFile(const std::string &path);
};

}  // namespace v4s
