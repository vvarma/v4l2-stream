#pragma once

#include <memory>
#include <vector>

#include "metadata.h"
#include "v4l/v4l_controls.h"
#include "v4l/v4l_device.h"
#include "v4l/v4l_frame.h"
namespace v4s {

class Algorithm {
 public:
  typedef std::shared_ptr<Algorithm> Ptr;

  virtual void Prepare(Metadata &metadata);
  virtual void ProcessStats(Metadata &metadata);
  virtual ~Algorithm() = default;

};

struct RegisterAlgorithm {
  typedef std::function<Algorithm::Ptr(Device::Ptr)> RegisterFn;
  RegisterAlgorithm(std::string name, RegisterFn factory);
};

Algorithm::Ptr GetAlgorithm(std::string name, Device::Ptr dev);

}  // namespace v4s
