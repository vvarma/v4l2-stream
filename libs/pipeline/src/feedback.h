#pragma once

#include <vector>

#include "algorithm.h"
#include "coro/async_generator.hpp"
#include "coro/task.hpp"
#include "v4l/v4l_device.h"
#include "v4l/v4l_stream.h"
namespace v4s {

struct Stats {
  float r_mean, g_mean, b_mean;
  void operator+=(const Stats& other);
};

class PipelineControl {
 public:
  PipelineControl(MMapStream::Ptr stats_source,
                  std::vector<Algorithm::Ptr> algorithms, Device::Ptr device);
  void ProcessStats();
  int GetFd() const;
  void Start();
  void Stop();
  Device::Ptr GetSource() const;
  typedef std::shared_ptr<PipelineControl> Ptr;

 private:
  MMapStream::Ptr stats_source_;
  std::vector<Algorithm::Ptr> algorithms_;
  Stats stats_;
  Device::Ptr device_;
};

}  // namespace v4s
