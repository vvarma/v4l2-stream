#pragma once

#include "coro/async_generator.hpp"
#include "coro/task.hpp"
#include "decoders.h"
#include "v4l/v4l_device.h"
#include "v4l/v4l_stream.h"
namespace v4s {

struct Stats {
  void operator+=(const Stats& other);
};

class PipelineControl {
 public:
  PipelineControl(MMapStream::Ptr stats_source, Decoder::Ptr decoder,
                  Device::Ptr device);
  void ProcessStats();
  int GetFd() const;
  void Start();
  void Stop();
  Device::Ptr GetSource() const;
  typedef std::shared_ptr<PipelineControl> Ptr;

 private:
  MMapStream::Ptr stats_source_;
  Decoder::Ptr decoder_;
  Stats stats_;
  Device::Ptr device_;
};

}  // namespace v4s
