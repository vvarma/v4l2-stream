#pragma once

#include <memory>
#include <string_view>
#include <unordered_map>

#include "pipeline/config.h"
#include "pipeline/pipeline.h"
#include "v4l/v4l_device.h"
namespace v4s {

class PipelineLoader {
 public:
  explicit PipelineLoader(PipelineConfig config);
  Pipeline Load() const;

 private:
  PipelineConfig config_;
};
}  // namespace v4s
