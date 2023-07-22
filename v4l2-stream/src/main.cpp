#include <chrono>
#include <csignal>
#include <spdlog/spdlog.h>
#include <thread>

#include "http-server/http-server.h"
#include "v4l/v4l_device.h"

#include "stream.h"

volatile std::atomic<bool> program_running = true;
void sigIntHandler(int signal) {
  spdlog::info("Got a signal {}", signal);
  program_running = false;
}

int main(int argc, char *argv[]) {
  // Install a signal handler
  std::signal(SIGINT, &sigIntHandler);
  std::signal(SIGTERM, &sigIntHandler);

  spdlog::set_level(spdlog::level::debug);
  auto device = v4s::Device::from_devnode("/dev/video0");
  v4s::Server server;
  server.RegisterRoute(StreamRoutes(device));
  // al tis was to handle the f'in address already in use
  // apparently that shit is not working still
  std::thread server_t([&server] { server.Start(); });
  while (program_running) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  server.Stop();
  server_t.join();
  return 0;
}
