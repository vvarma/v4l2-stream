#include <chrono>
#include <csignal>
#include <memory>
#include <thread>
#include <vector>

#include <spdlog/spdlog.h>
#include <structopt/app.hpp>

#include "http-server/http-server.h"
#include "pipeline/config.h"
#include "pipeline/loader.h"
#include "v4l/v4l_bridge.h"
#include "v4l/v4l_capture.h"
#include "v4l/v4l_device.h"
#include "v4l/v4l_stream.h"

#include "stream.h"

volatile std::atomic<bool> program_running = true;
void sigIntHandler(int signal) {
  spdlog::info("Got a signal {}", signal);
  program_running = false;
}

struct Options {
  std::string pipeline_config;
};
STRUCTOPT(Options, pipeline_config);

int main(int argc, char *argv[]) {
  try {
    auto options = structopt::app("v4l2-stream").parse<Options>(argc, argv);
    // Install a signal handler
    std::signal(SIGINT, &sigIntHandler);
    std::signal(SIGTERM, &sigIntHandler);

    spdlog::set_level(spdlog::level::trace);
    v4s::PipelineLoader loader(
        v4s::PipelineConfig::FromFile(options.pipeline_config));
    v4s::Server server;
    server.RegisterRoute(StreamRoutes(loader));
    // al tis was to handle the f'in address already in use
    // apparently that shit is not working still
    std::thread server_t([&server] { server.Start(); });
    while (program_running) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    server.Stop();
    server_t.join();
    return 0;
  } catch (structopt::exception &e) {
    spdlog::error("Exception parsing cli parameters {}", e.what());
    spdlog::info("{}", e.help());
  }
  return 1;
}
