#include "metrics/metrics.h"

#include <atomic>
#include <chrono>
#include <mutex>

namespace v4s {

void Stat::Add(std::string_view name, value_t value) {
  stats_.emplace(name, value);
}

Counter::Counter(std::string_view name) : name_(name), value_(0) {}
void Counter::Report(Stat &stat) const {
  stat.Add(name_, value_.load(std::memory_order_consume));
}
void Counter::Increment() { value_.fetch_add(1, std::memory_order_release); }
void Counter::Decrement() { value_.fetch_sub(1, std::memory_order_release); }

Counter::Ptr Counter::GetCounter(std::string_view name) {
  static std::unordered_map<std::string, Counter::Ptr> counters;
  static std::mutex mutex;
  auto it = counters.find(std::string(name));
  if (it != counters.end()) {
    return it->second;
  }
  std::lock_guard<std::mutex> lock(mutex);
  it = counters.find(std::string(name));
  if (it != counters.end()) {
    return it->second;
  }
  auto counter = std::make_shared<Counter>(name);
  Metrics::GetInstance().RegisterMetric(counter);
  counters.emplace(name, counter);
  return counter;
}

RollingCounter::RollingCounter(std::string_view name)
    : name_(name), buckets_(60), curr_(0), total_(0) {}
void RollingCounter::Report(Stat &stat) const {
  stat.Add(name_ + "_total", total_.load(std::memory_order_consume));
}

Metrics &Metrics::GetInstance() {
  static Metrics instance;
  return instance;
}
void Metrics::RegisterMetric(Metric::Ptr metric) {
  std::lock_guard<std::mutex> lock(mutex_);
  metrics_.emplace_back(metric);
}

Stat Metrics::Report() {
  Stat stat;
  for (auto &metric : metrics_) {
    metric->Report(stat);
  }
  return stat;
}
void to_json(nlohmann::json &j, const Stat &p) {
  j = nlohmann::json{};
  for (auto &it : p.stats_) {
    if (std::holds_alternative<int64_t>(it.second)) {
      j[it.first] = std::get<int64_t>(it.second);
    } else {
      j[it.first] = std::get<double>(it.second);
    }
  }
}

}  // namespace v4s
