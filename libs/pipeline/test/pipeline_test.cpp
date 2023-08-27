#include "pipeline/pipeline.h"

#include <spdlog/spdlog.h>

#include <chrono>
#include <iostream>
#include <stop_token>
#include <thread>

#include "metrics/metrics.h"
#include "pipeline/config.h"
#include "pipeline/loader.h"
using namespace std::chrono_literals;
int main() {
  auto pipeline =
      v4s::PipelineLoader(
          v4s::PipelineConfig{
              .bridges = {v4s::BridgeConfig{.source = "/dev/video0",
                                            .sink = "/dev/video12"},
                          v4s::BridgeConfig{.source = "/dev/video12",
                                            .sink = "/dev/video11"}},
              .source = {.source = "/dev/video11"},
          })
          .Load();
  std::jthread pipeline_thread(
      [&](std::stop_token stop_token) { pipeline.Start(stop_token); });

  std::jthread metrics_thread([](std::stop_token stop_token) {
    auto &metrics = v4s::Metrics::GetInstance();
    while (!stop_token.stop_requested()) {
      std::this_thread::sleep_for(1s);
      nlohmann::json j = metrics.Report();
      std::cout << j.dump() << std::endl;
    }
  });
  auto start = std::chrono::high_resolution_clock::now();
  pipeline.Prepare("MJPG");

  for (int i = 0; i < 1000; ++i) {
    auto frame = pipeline.Next();
    spdlog::info("got frame {}", i);
  }
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - start);
  std::cout << "1000 frames in " << duration.count() << "ms" << std::endl;
}
