#include "feedback.h"

namespace v4s {
PipelineControl::PipelineControl(MMapStream::Ptr stats_source,
                                 Decoder::Ptr decoder, Device::Ptr device)
    : stats_source_(std::move(stats_source)),
      decoder_(std::move(decoder)),
      device_(std::move(device)) {}
int PipelineControl::GetFd() const { return stats_source_->GetFd(); }

void PipelineControl::ProcessStats() {
  auto frame = stats_source_->FetchNext();
  decoder_->Decode(frame, stats_);
}

void PipelineControl::Start() { stats_source_->Start(); }

Device::Ptr PipelineControl::GetSource() const {
  return stats_source_->GetDevice();
}

}  // namespace v4s
