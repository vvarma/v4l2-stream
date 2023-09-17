#include "decoder.h"

#include <spdlog/spdlog.h>

#include <unordered_map>

namespace v4s {
std::unordered_map<std::string, DecoderFn>& GetDecoders() {
  static std::unordered_map<std::string, DecoderFn> decoders;
  return decoders;
}
RegisterDecoder::RegisterDecoder(std::string name, DecoderFn factory) {
  spdlog::info("Registering decoder {}", name);
  GetDecoders()[name] = factory;
}
std::optional<DecoderFn> GetDecoder(std::string name) {
  if (GetDecoders().find(name) == GetDecoders().end()) {
    return std::nullopt;
  }
  return GetDecoders()[name];
}

}  // namespace v4s
