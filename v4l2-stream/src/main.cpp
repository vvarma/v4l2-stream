#include <http-server/http-server.h>
#include <http-server/static-routes.h>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>

#include <asio.hpp>
#include <asio/use_awaitable.hpp>
#include <chrono>
#include <coro/sync_wait.hpp>
#include <csignal>
#include <memory>
#include <structopt/app.hpp>
#include <thread>
#include <vector>

#include "ctrls.h"
#include "metrics.h"
#include "pipeline/config.h"
#include "pipeline/loader.h"
#include "selection.h"
#include "snapshot.h"
#include "stream.h"
#include "v4l/v4l_bridge.h"
#include "v4l/v4l_capture.h"
#include "v4l/v4l_device.h"
#include "v4l/v4l_stream.h"

using namespace std::chrono_literals;

struct Options {
  std::string pipeline_config;
  std::optional<spdlog::level::level_enum> log_level = spdlog::level::info;
  std::optional<std::string> web_path;
};
STRUCTOPT(Options, pipeline_config, log_level, web_path);

int main(int argc, char *argv[]) {
  try {
    auto options = structopt::app("v4l2-stream").parse<Options>(argc, argv);
    spdlog::set_level(options.log_level.value());

    asio::io_context io_context(1);
    v4s::PipelineLoader loader(v4s::PipelineConfig::FromFile(options.pipeline_config));
    auto server = std::make_shared<hs::HttpServer>(
        hs::Config("v4l2-stream", "0.0.0.0", 4891));

    server->AddRoute(StreamRoutes(loader));
    server->AddRoute(SnapshotRoutes(loader));
    for (auto &route : SelectionRoutes(loader)) {
      server->AddRoute(route);
    }
    for (auto &route : CtrlRoutes(loader)) {
      server->AddRoute(route);
    }
    server->AddRoute(MetricsRoute());
    if (options.web_path)
      server->AddRoute(
          std::make_shared<hs::StaticRoute>("/", options.web_path.value()));

    std::jthread t([&]() { coro::sync_wait(server->ServeAsync(io_context)); });
    std::this_thread::sleep_for(1s);
    // signals.async_wait([&](auto, auto) { io_context.stop(); });
    io_context.run();
    return 0;
  } catch (structopt::exception &e) {
    spdlog::error("Exception parsing cli parameters {}", e.what());
    spdlog::info("{}", e.help());
  }
  return 1;
}
