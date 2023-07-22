#include "encoder/mjpeg_encoder.h"
#include "http-server/http-server.h"
#include "pipeline/pipeline.h"
#include "v4l/v4l_capture.h"
#include "v4l/v4l_device.h"

#include <exception>
#include <spdlog/spdlog.h>

struct StreamRoute : public v4s::Route {
  v4s::Device::Ptr device_;
  StreamRoute(v4s::Device::Ptr device) : device_(device) {}
  std::string Path() { return "/stream"; }
  void Handle(v4s::Request req, v4s::ResponseWriter rw) {
    try {
      spdlog::info("Got a request to stream");
      v4s::MJpegEncoder encoder;
      auto stream = device_->TryCapture()->Stream();
      v4s::Pipeline<v4s::MJpegEncoder> pipeline(stream, encoder);
      rw.SetContentType(encoder.ContentType());
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

v4s::Route::Ptr StreamRoutes(v4s::Device::Ptr device) {
  return std::make_shared<StreamRoute>(device);
}
