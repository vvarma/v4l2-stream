#include "encoder/mjpeg_encoder.h"
#include "http-server/http-server.h"
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
  v4s::Pipeline<v4s::MJpegEncoder> pipeline_;
  StreamRoute(std::vector<v4s::Bridge::Ptr> bridges,
              v4s::MMapStream::Ptr stream)
      : pipeline_(bridges, stream, encoder_) {}
  std::string Path() { return "/stream"; }
  void Handle(v4s::Request req, v4s::ResponseWriter rw) {
    try {
      spdlog::info("Got a request to stream");
      pipeline_.Start();
      v4s::MJpegEncoder encoder;
      rw.SetContentType(encoder.ContentType());
      while (true) {
        auto frame = pipeline_.Next();
        spdlog::debug("got a frame");
        rw.Write(frame);
      }
    } catch (std::exception &e) {
      spdlog::error("got an exception {}", e.what());
    }
  }
};

v4s::Route::Ptr StreamRoutes(std::vector<v4s::Bridge::Ptr> bridges,
                             v4s::MMapStream::Ptr stream) {
  return std::make_shared<StreamRoute>(bridges, stream);
}
