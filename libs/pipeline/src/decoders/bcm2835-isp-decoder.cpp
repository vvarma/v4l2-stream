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
  int64_t r_awb_sum = 0, g_awb_sum = 0, b_awb_sum = 0;
  int64_t num_pixels = 0;
  for (int i = 0; i < AWB_REGIONS; ++i) {
    r_awb_sum += stats_ptr->awb_stats[i].r_sum;
    g_awb_sum += stats_ptr->awb_stats[i].g_sum;
    b_awb_sum += stats_ptr->awb_stats[i].b_sum;
    num_pixels += stats_ptr->awb_stats[i].counted;
    metadata.Set("r_sum", r_awb_sum);
    metadata.Set("g_sum", g_awb_sum);
    metadata.Set("b_sum", b_awb_sum);
    metadata.Set("num_pixels", num_pixels);
  }
}

static v4s::RegisterDecoder reg("bcm2835-isp", BCM2835IspStatsDecode);

}  // namespace
