#pragma once

#include <atomic>
#include <cstdint>
#include <deque>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>
namespace v4s {
struct Stat {
  typedef std::variant<int64_t, double> value_t;
  void Add(std::string_view name, value_t value);
  std::unordered_map<std::string, value_t> stats_;
};
void to_json(nlohmann::json &j, const Stat &p);

class Metric {
 public:
  typedef std::shared_ptr<Metric> Ptr;
  virtual ~Metric() = default;
  virtual void Report(Stat &stat) const = 0;
};

class Counter : public Metric {
 public:
  typedef std::shared_ptr<Counter> Ptr;
  static Counter::Ptr GetCounter(std::string_view name);
  void Report(Stat &stat) const override;
  void Increment();
  void Decrement();
  Counter(std::string_view name);

 private:
  std::string name_;
  std::atomic_int64_t value_;
};

class RollingCounter : public Metric {
 public:
  typedef std::shared_ptr<RollingCounter> Ptr;
  static RollingCounter::Ptr GetCounter(std::string_view name);
  void Report(Stat &stat) const override;
  void Increment();

 private:
  RollingCounter(std::string_view name);
  std::string name_;
  std::deque<int64_t> buckets_;
  std::atomic_int64_t curr_, total_;
};

class Metrics {
 public:
  static Metrics &GetInstance();
  void RegisterMetric(Metric::Ptr metric);
  Stat Report();

 private:
  std::mutex mutex_;
  std::vector<Metric::Ptr> metrics_;
};
}  // namespace v4s
