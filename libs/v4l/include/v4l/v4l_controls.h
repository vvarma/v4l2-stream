#ifndef V4L2_STREAM_V4L_CONTROLS_H
#define V4L2_STREAM_V4L_CONTROLS_H

#include <cstdint>
#include <memory>
#include <nlohmann/json.hpp>
#include <variant>
namespace v4s {

struct IntControl {
  IntControl();
  IntControl(uint32_t id, const std::string &name, int64_t min, int64_t max,
             int64_t def, int64_t curr, uint64_t step);

  void UpdateCurrent(int fd);
  void SetControl(int fd, int64_t val);
  nlohmann::json ToJson() const;

  uint32_t id;
  std::string name;
  int64_t min, max, def, curr;
  uint64_t step;
};

}  // namespace v4s

#endif  // !#ifndef V4L2_STREAM_V4L_CONTROLS_H
