#ifndef V4L2_STREAM_PIPELINE_CONFIG_H
#define V4L2_STREAM_PIPELINE_CONFIG_H

#include <string>
#include <vector>
namespace v4s {
struct PipelineConfig {
  std::vector<std::string> devices;
  static PipelineConfig FromFile(const std::string &path);
};

}

#endif // !V4L2_STREAM_PIPELINE_CONFIG_H
