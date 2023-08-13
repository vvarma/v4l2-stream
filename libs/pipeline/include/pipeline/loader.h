#ifndef V4L2_STREAM_PIPELINE_LOADER_H
#define V4L2_STREAM_PIPELINE_LOADER_H

#include <memory>

#include "pipeline/config.h"
#include "pipeline/pipeline.h"
#include "v4l/v4l_device.h"
namespace v4s {

class PipelineLoader {
 public:
  explicit PipelineLoader(PipelineConfig config);
  Pipeline Load() const;

 private:
  std::vector<Device::Ptr> devices_;
};
}  // namespace v4s

#endif  // !#ifndef V4L2_STREAM_PIPELINE_LOADER_H
