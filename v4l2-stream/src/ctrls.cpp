#include "ctrls.h"

#include <memory>
#include <nlohmann/json.hpp>

#include "http-server/enum.h"
#include "http-server/route.h"
#include "pipeline/pipeline.h"
#include "v4l/v4l_controls.h"
using json = nlohmann::json;
using namespace nlohmann::literals;

struct GetResp {
  std::vector<v4s::Control::Ptr> controls;
  json ToJson() const {
    json j = json::array();
    for (auto &ctrl : controls) {
      j.push_back(ctrl->ToJson());
    }
    return j;
  }
};

struct GetCtrlsHandler : public hs::Handler {
  hs::Generator<hs::Response> Handle(const hs::Request &req) override {
    auto ctrls = pipeline_.GetSource()->GetControls();
    co_yield hs::StatusCode::Ok;
    auto resp = GetResp(ctrls).ToJson().dump();
    hs::Headers headers = {
        {"Content-Type", "application/json"},
        {"Content-Length", fmt::format("{}", resp.size())},
    };
    co_yield headers;
    co_yield std::make_shared<hs::WritableResponseBody<std::string>>(resp);
  }
  GetCtrlsHandler(v4s::Pipeline pipeline) : pipeline_(pipeline) {}
  v4s::Pipeline pipeline_;
};

struct GetCtrlsRoute : public hs::Route {
  hs::Method GetMethod() const override { return hs::Method::GET; }
  std::string GetPath() const override { return "/ctrls"; }
  hs::Handler::Ptr GetHandler() const override {
    return std::make_shared<GetCtrlsHandler>(pipeline_);
  }
  GetCtrlsRoute(const v4s::Pipeline &pipeline) : pipeline_(pipeline) {}
  v4s::Pipeline pipeline_;
};

std::vector<hs::Route::Ptr> CtrlRoutes(v4s::Pipeline pipeline) {
  return {std::make_shared<GetCtrlsRoute>(pipeline)};
}
