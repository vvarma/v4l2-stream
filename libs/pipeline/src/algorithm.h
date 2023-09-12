#pragma once

#include <memory>

#include "v4l/v4l_frame.h"
namespace v4s {

class Algorithm {
 public:
  typedef std::shared_ptr<Algorithm> Ptr;
  virtual void ProcessStats(const Frame::Ptr frame) = 0;
  virtual ~Algorithm() = default;
};

struct RegisterAlgorithm {
  RegisterAlgorithm(std::string name, std::function<Algorithm::Ptr()> factory);
};

Algorithm::Ptr GetAlgorithm(std::string name);

}  // namespace v4s
