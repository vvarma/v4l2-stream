#include "feedback.h"

#include <math.h>
#include <spdlog/spdlog.h>

#include <vector>

#include "algorithm.h"

namespace v4s {
PipelineControl::PipelineControl(MMapStream::Ptr stats_source,
                                 std::vector<Algorithm::Ptr> algorithms,
                                 Device::Ptr device)
    : stats_source_(std::move(stats_source)),
      algorithms_(algorithms),
      device_(std::move(device)) {}
int PipelineControl::GetFd() const { return stats_source_->GetFd(); }

void PipelineControl::ProcessStats() {
  auto frame = stats_source_->FetchNext();
  for (auto& algorithm : algorithms_) {
    algorithm->ProcessStats(frame);
  }
}

void PipelineControl::Start() { stats_source_->Start(); }
void PipelineControl::Stop() { stats_source_->Stop(); }

Device::Ptr PipelineControl::GetSource() const {
  return stats_source_->GetDevice();
}

}  // namespace v4s
