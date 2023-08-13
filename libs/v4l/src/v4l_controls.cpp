#include "v4l/v4l_controls.h"

#include <fmt/format.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>

#include <cstdint>
#include <cstring>
#include <vector>

#include "v4l/v4l_exception.h"

namespace v4s {
Control::Control() {}
Control::Control(uint32_t id, const std::string &name) : id(id), name(name) {}
void Control::SetControl(int, int64_t) {
  throw Exception("Setting control(int64_t) not supported");
}
Control::~Control() {}

IntControl::IntControl() {}
IntControl::IntControl(uint32_t id, const std::string &name, int64_t min,
                       int64_t max, int64_t def, int64_t curr, uint64_t step)
    : Control(id, name), min(min), max(max), def(def), curr(curr), step(step) {}

void IntControl::UpdateCurrent(int fd) {
  v4l2_ext_controls controls;
  memset(&controls, 0, sizeof(controls));
  controls.count = 1;
  controls.which = V4L2_CTRL_WHICH_CUR_VAL;
  std::vector<v4l2_ext_control> control(1);
  memset(&control[0], 0, sizeof(control[0]));
  control[0].id = id;
  controls.controls = control.data();
  int ret = ioctl(fd, VIDIOC_G_EXT_CTRLS, &controls);
  if (ret < 0) {
    throw Exception(fmt::format("Error getting control {}", strerror(errno)));
  }
  curr = control[0].value64;
}
void IntControl::SetControl(int fd, int64_t val) {
  v4l2_ext_controls controls;
  memset(&controls, 0, sizeof(controls));
  controls.count = 1;
  controls.which = V4L2_CTRL_WHICH_CUR_VAL;
  std::vector<v4l2_ext_control> control(1);
  memset(&control[0], 0, sizeof(control[0]));
  control[0].id = id;
  control[0].value64 = val;
  controls.controls = control.data();
  int ret = ioctl(fd, VIDIOC_S_EXT_CTRLS, &controls);
  if (ret < 0) {
    throw Exception(fmt::format("Error setting control {}", strerror(errno)));
  }
  curr = control[0].value64;
}
nlohmann::json IntControl::ToJson() const {
  return {
      {"id", id},   {"name", name}, {"min", min},   {"max", max},
      {"def", def}, {"curr", curr}, {"step", step},
  };
}
}  // namespace v4s
