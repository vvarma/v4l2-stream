#include "ctrls.h"

#include <cstdint>
#include <memory>
#include <nlohmann/detail/macro_scope.hpp>
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
  coro::async_generator<hs::Response> Handle(const hs::Request req) override {
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

struct SetCtrlReq {
  struct Ctrl {
    uint32_t id;
    int64_t val;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Ctrl, id, val);
  };
  std::vector<Ctrl> controls;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(SetCtrlReq, controls);
};

struct SetCtrlsHandler : public hs::Handler {
  coro::async_generator<hs::Response> Handle(const hs::Request req) override {
    json body = json::parse(co_await req.Body());
    SetCtrlReq set_ctrl_req = body;
    for (const auto &ctrl : set_ctrl_req.controls) {
      pipeline_.GetSource()->SetControl(ctrl.id, ctrl.val);
    }
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
  SetCtrlsHandler(v4s::Pipeline pipeline) : pipeline_(pipeline) {}
  v4s::Pipeline pipeline_;
};

struct SetCtrlsRoute : public hs::Route {
  hs::Method GetMethod() const override { return hs::Method::POST; }
  std::string GetPath() const override { return "/ctrls"; }
  hs::Handler::Ptr GetHandler() const override {
    return std::make_shared<SetCtrlsHandler>(pipeline_);
  }
  SetCtrlsRoute(const v4s::Pipeline &pipeline) : pipeline_(pipeline) {}
  v4s::Pipeline pipeline_;
};

std::vector<hs::Route::Ptr> CtrlRoutes(v4s::Pipeline pipeline) {
  return {std::make_shared<GetCtrlsRoute>(pipeline),
          std::make_shared<SetCtrlsRoute>(pipeline)};
}
