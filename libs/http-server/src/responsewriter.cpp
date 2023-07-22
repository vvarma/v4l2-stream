#include "responsewriter.h"
#include "http-server/http-server.h"

#include <cstdint>
#include <memory>

#include <pistache/http.h>
#include <pistache/http_defs.h>
#include <pistache/http_header.h>
#include <spdlog/spdlog.h>
#include <vector>

namespace v4s {
namespace internal {
ResponseWriterImpl::ResponseWriterImpl(P::Http::ResponseWriter writer)
    : writer_(std::move(writer)) {}
void ResponseWriterImpl::SetContentType(const std::string &content_type) {
  writer_.setMime(P::Http::Mime::MediaType(content_type));
}
void ResponseWriterImpl::Write(std::vector<uint8_t> &body) {
  if (!stream_)
    stream_ = writer_.stream(P::Http::Code::Ok);
  stream_->write(reinterpret_cast<char *>(body.data()), body.size());
  stream_->flush();
}
ResponseWriterImpl::~ResponseWriterImpl() {
  spdlog::info("closing responsewriter");
  if (!stream_) {
    spdlog::info("no stream");
    writer_.send(P::Http::Code::Gone);
  } else {
    spdlog::info("end stream");
    stream_->ends();
  }
}
} // namespace internal
ResponseWriter::ResponseWriter(
    std::shared_ptr<internal::ResponseWriterImpl> pimpl)
    : pimpl_(std::move(pimpl)) {}
void ResponseWriter::SetContentType(const std::string &content_type) {
  pimpl_->SetContentType(content_type);
}
void ResponseWriter::Write(std::vector<uint8_t> &body) { pimpl_->Write(body); }
ResponseWriter::~ResponseWriter() {}
} // namespace v4s
