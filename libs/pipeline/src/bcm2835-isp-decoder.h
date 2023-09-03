#pragma once

#include "decoders.h"

namespace v4s {
class BCM2835IspStatsDecoder : public Decoder {
 public:
  void Decode(Frame::Ptr frame, Stats &stats) override;
};

}  // namespace v4s
