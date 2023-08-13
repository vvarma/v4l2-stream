#ifndef V4L2_STREAM_V4L_CONTROLS_H
#define V4L2_STREAM_V4L_CONTROLS_H

#include <cstdint>
#include <memory>
#include <nlohmann/json.hpp>
#include <variant>
namespace v4s {

struct Control {
  virtual void UpdateCurrent(int fd) = 0;
  typedef std::shared_ptr<Control> Ptr;
  Control();
  Control(uint32_t id, const std::string &name);
  virtual void SetControl(int fd, int64_t val);
  virtual ~Control();
  virtual nlohmann::json ToJson() const = 0;

  uint32_t id;
  std::string name;
};

struct IntControl : public Control {
  IntControl();
  IntControl(uint32_t id, const std::string &name, int64_t min, int64_t max,
             int64_t def, int64_t curr, uint64_t step);

  void UpdateCurrent(int fd) override;
  void SetControl(int fd, int64_t val) override;
  nlohmann::json ToJson() const override;

  int64_t min, max, def, curr;
  uint64_t step;
};

}  // namespace v4s

#endif  // !#ifndef V4L2_STREAM_V4L_CONTROLS_H
