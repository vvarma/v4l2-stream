#include "decoders.h"

#include <optional>

#include "bcm2835-isp-decoder.h"

namespace v4s {
std::optional<Decoder::Ptr> Decoders::Get(std::string_view name) {
  if (name == "bcm2835_isp_stats") {
    return std::make_shared<BCM2835IspStatsDecoder>();
  }
  return std::nullopt;
}
}  // namespace v4s
