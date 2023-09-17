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

struct AlgoConfig {
  std::string name;
  std::string device;
};
struct StatsSourceConfig {
  std::string source, codec, decoder;
};
struct ControlConfig {
  StatsSourceConfig stats_device;
  std::vector<AlgoConfig> algorithms;
};

struct PipelineConfig {
  std::vector<BridgeConfig> bridges;
  SourceConfig source;
  std::vector<ControlConfig> controls;

  static PipelineConfig FromFile(const std::string &path);
};

}  // namespace v4s
