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
struct PipelineConfig {
  std::vector<BridgeConfig> bridges;
  SourceConfig source;
  static PipelineConfig FromFile(const std::string &path);
};

}  // namespace v4s
