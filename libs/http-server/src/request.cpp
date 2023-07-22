#include "request.h"
#include "http-server/http-server.h"
#include <memory>
#include <pistache/router.h>

namespace v4s {
namespace internal {
RequestImpl::RequestImpl(const P::Rest::Request &req) : req(req) {}

} // namespace internal

Request::Request(std::shared_ptr<internal::RequestImpl> pimpl)
    : pimpl_(std::move(pimpl)) {}
Request::~Request() {}

} // namespace v4s
