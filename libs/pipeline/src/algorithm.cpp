#include "algorithm"

#include <spdlog/spdlog.h>

#include <map>

#include "algorithm.h"
#include "metadata.h"
namespace v4s {

void Algorithm::Prepare(Metadata&){};
void Algorithm::ProcessStats(Metadata&){};

static std::map<std::string, RegisterAlgorithm::RegisterFn>& algorithms() {
  static std::map<std::string, RegisterAlgorithm::RegisterFn> algorithms;
  return algorithms;
}
RegisterAlgorithm::RegisterAlgorithm(std::string name,
                                     RegisterAlgorithm::RegisterFn factory) {
  spdlog::info("Registering algorithm {}", name);
  algorithms().emplace(name, factory);
}

Algorithm::Ptr GetAlgorithm(std::string name, Device::Ptr dev) {
  if (algorithms().find(name) == algorithms().end()) {
    spdlog::error("Algorithm {} not found", name);
    return nullptr;
  }
  return algorithms()[name](dev);
}

}  // namespace v4s
