#pragma once

#include <memory>
#include <optional>
#include <unordered_map>

#include "v4l/v4l_frame.h"

namespace v4s {
struct Stats;
class Decoder {
 public:
  typedef std::shared_ptr<Decoder> Ptr;
  virtual void Decode(Frame::Ptr frame, Stats &stats) = 0;
  virtual ~Decoder(){};
};

class Decoders {
 public:
  static std::optional<Decoder::Ptr> Get(std::string_view name);
};

}  // namespace v4s
