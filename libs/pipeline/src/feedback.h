#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "algorithm.h"
#include "coro/async_generator.hpp"
#include "coro/task.hpp"
#include "decoder.h"
#include "v4l/v4l_device.h"
#include "v4l/v4l_stream.h"
namespace v4s {

struct StatsSource {
  MMapStream::Ptr stream;
  DecoderFn decoder;
  std::string codec;
};

class PipelineControl {
 public:
  PipelineControl(StatsSource stats_source,
                  std::vector<Algorithm::Ptr> algorithms);
  void ProcessStats();
  int GetFd() const;
  void Start();
  void Stop();
  typedef std::shared_ptr<PipelineControl> Ptr;

 private:
  StatsSource stats_source_;
  std::vector<Algorithm::Ptr> algorithms_;
};

}  // namespace v4s
