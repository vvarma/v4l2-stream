#include "bcm2835-isp-decoder.h"

#include <spdlog/spdlog.h>

#include "decoders.h"
#include "decoders/bcm2835-isp.h"

namespace v4s {
void BCM2835IspStatsDecoder::Decode(Frame::Ptr frame, Stats &stats) {
  auto stats_ptr = static_cast<const bcm2835_isp_stats *>(frame->Data(0));
  spdlog::info("Stats: {0}", stats_ptr->version);
}

}  // namespace v4s
