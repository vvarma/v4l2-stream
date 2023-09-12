#include "algorithm.h"

namespace {
class RpiAgc : public v4s::Algorithm {
 public:
  RpiAgc() = default;
  ~RpiAgc() override = default;
  void ProcessStats(v4s::Frame::Ptr frame) override {}
};

v4s::RegisterAlgorithm reg("rpi-agc",
                           []() { return std::make_shared<RpiAgc>(); });

}  // namespace
