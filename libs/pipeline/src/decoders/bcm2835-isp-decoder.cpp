#include "bcm2835-isp-decoder.h"

#include <spdlog/spdlog.h>

#include <cstdint>

#include "bcm2835-isp.h"

namespace v4s {
bcm2835_isp_stats BCM2835IspStatsDecode(Frame::Ptr frame) {
  auto stats_ptr = static_cast<const bcm2835_isp_stats *>(frame->Data(0));
  bcm2835_isp_stats stats = *stats_ptr;
  return stats;
}

}  // namespace v4s
