#pragma once
#include <memory>
#include <nlohmann/detail/macro_scope.hpp>
#include <optional>
#include <stop_token>
#include <vector>

#include "v4l/v4l_device.h"
#include "v4l/v4l_frame.h"
namespace v4s {

enum Role { SOURCE, SINK, STATS };

NLOHMANN_JSON_SERIALIZE_ENUM(Role, {{SOURCE, "source"},
                                    {SINK, "sink"},
                                    {STATS, "stats"}});

struct Stage {
  Role role;
  std::string dev_node;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Stage, role, dev_node);
};

struct PipelineDesc {
  std::vector<Stage> stages;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(PipelineDesc, stages);
};

namespace internal {
class PipelineImpl;
}  // namespace internal

class Pipeline {
 public:
  v4s::Frame::Ptr Next();
  explicit Pipeline(std::shared_ptr<internal::PipelineImpl> pimpl);
  ~Pipeline();
  void Prepare(std::string sink_codec);
  void Start(std::stop_token stop_token);
  PipelineDesc GetDesc() const;
  std::optional<Device::Ptr> GetDevice(std::string_view dev_node) const;

 private:
  std::shared_ptr<internal::PipelineImpl> pimpl_;
};

}  // namespace v4s
