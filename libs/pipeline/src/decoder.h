#pragma once

#include <functional>
#include <optional>

#include "metadata.h"
#include "v4l/v4l_frame.h"
namespace v4s {

typedef std::function<void(Frame::Ptr, Metadata&)> DecoderFn;

struct RegisterDecoder {
  RegisterDecoder(std::string name, DecoderFn factory);
};

std::optional<DecoderFn> GetDecoder(std::string name);

}  // namespace v4s
