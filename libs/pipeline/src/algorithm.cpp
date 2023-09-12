#include "algorithm"

#include <spdlog/spdlog.h>

#include <map>

#include "algorithm.h"
namespace v4s {
static std::map<std::string, Algorithm::Ptr>& algorithms() {
  static std::map<std::string, Algorithm::Ptr> algorithms;
  return algorithms;
}
RegisterAlgorithm::RegisterAlgorithm(std::string name,
                                     std::function<Algorithm::Ptr()> factory) {
  spdlog::info("Registering algorithm {}", name);
  algorithms().emplace(name, factory());
}

Algorithm::Ptr GetAlgorithm(std::string name) { return algorithms()[name]; }

}  // namespace v4s
