#include "responsewriter.h"
#include "http-server/http-server.h"

#include <cstdint>
#include <memory>

#include <spdlog/spdlog.h>
#include <vector>

namespace v4s {
namespace internal {
void ResponseWriterImpl::SetContentType(const std::string &content_type) {
  auto line =
      fmt::format("HTTP/1.1 200 OK\r\nContent-Type: {}\r\n\r\n", content_type);
  socket_.write_some(asio::buffer(line));
}
void ResponseWriterImpl::Write(std::vector<uint8_t> &body) {
  socket_.write_some(asio::buffer(body));
}
ResponseWriterImpl::ResponseWriterImpl(asio::ip::tcp::socket &socket)
    : socket_(socket) {}
ResponseWriterImpl::~ResponseWriterImpl() {}
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
