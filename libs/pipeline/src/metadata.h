#pragma once
#include <any>
#include <string>
#include <unordered_map>
namespace v4s {

class Metadata {
 public:
  template <typename T>
  void Set(std::string key, T value) {
    data_[key] = value;
  }
  template <typename T>
  T Get(std::string key) {
    return std::any_cast<T>(data_[key]);
  }

 private:
  std::unordered_map<std::string, std::any> data_;
};
}  // namespace v4s
