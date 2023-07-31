#include "encoder/mjpeg_encoder.h"
#include "http-server/http-server.h"
#include "pipeline/loader.h"
#include "pipeline/pipeline.h"
#include "v4l/v4l_bridge.h"
#include "v4l/v4l_capture.h"
#include "v4l/v4l_device.h"
#include "v4l/v4l_stream.h"

#include <exception>
#include <spdlog/spdlog.h>
#include <vector>

struct StreamRoute : public v4s::Route {
  v4s::MJpegEncoder encoder_;
  v4s::PipelineLoader loader_;
  StreamRoute(v4s::PipelineLoader loader) : loader_(loader) {}
  std::string Path() { return "/stream"; }
  void Handle(v4s::Request req, v4s::ResponseWriter rw) {
    auto pipeline = loader_.Load(encoder_);

    try {
      std::thread pipelineThread([&] { pipeline.Start(); });
      v4s::MJpegEncoder encoder;
      rw.SetContentType(encoder.ContentType());
      spdlog::debug("starting loop");
      while (true) {
        auto frame = pipeline.Next();
        spdlog::debug("got a frame");
        rw.Write(frame);
      }
    } catch (std::exception &e) {
      spdlog::error("got an exception {}", e.what());
    }
  }
};

v4s::Route::Ptr StreamRoutes(v4s::PipelineLoader loader) {
  return std::make_shared<StreamRoute>(loader);
}
