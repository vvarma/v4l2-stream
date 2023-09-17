#include <spdlog/spdlog.h>

#include <cstdint>

#include "../decoder.h"
#include "bcm2835-isp.h"

namespace {
void BCM2835IspStatsDecode(v4s::Frame::Ptr frame, v4s::Metadata &metadata) {
  auto stats_ptr = static_cast<const bcm2835_isp_stats *>(frame->Data(0));
  std::vector<int> hist(stats_ptr->hist[0].g_hist,
                        std::end(stats_ptr->hist[0].g_hist));
  metadata.Set("rpi.g_hist", hist);
}

static v4s::RegisterDecoder reg("bcm2835-isp", BCM2835IspStatsDecode);

}  // namespace
