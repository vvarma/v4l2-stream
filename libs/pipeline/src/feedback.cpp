#include "feedback.h"

#include <math.h>
#include <spdlog/spdlog.h>

#include <vector>

#include "algorithm.h"
#include "decoder.h"
#include "metadata.h"

namespace v4s {
PipelineControl::PipelineControl(StatsSource stats_source,
                                 std::vector<Algorithm::Ptr> algorithms)
    : stats_source_(std::move(stats_source)), algorithms_(algorithms) {}

int PipelineControl::GetFd() const { return stats_source_.stream->GetFd(); }

void PipelineControl::ProcessStats() {
  // for each call?
  v4s::Metadata metadata;
  auto frame = stats_source_.stream->Next();
  stats_source_.decoder(frame, metadata);

  for (auto& algorithm : algorithms_) {
    algorithm->ProcessStats(metadata);
  }
}

void PipelineControl::Start() {
  stats_source_.stream->GetDevice()->SetFormat(BUF_META_CAPTURE,
                                               Format{
                                                   .codec = stats_source_.codec,
                                               });
  stats_source_.stream->Start();
  v4s::Metadata metadata;
  for (auto& algorithm : algorithms_) {
    algorithm->Prepare(metadata);
  }
}
void PipelineControl::Stop() { stats_source_.stream->Stop(); }

}  // namespace v4s
