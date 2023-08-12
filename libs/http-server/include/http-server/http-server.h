#ifndef V4L2_STREAM_HTTP_SERVER_H
#define V4L2_STREAM_HTTP_SERVER_H

#include <asio/awaitable.hpp>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <sys/types.h>
#include <vector>

#include <asio.hpp>

namespace v4s {

namespace internal {
class ServerImpl;
class RequestImpl;
class ResponseWriterImpl;
} // namespace internal

struct ServerOpts {};

class Request {
public:
  template <typename T> T Query(const std::string &key);
  explicit Request(std::shared_ptr<internal::RequestImpl> pimpl);
  ~Request();

private:
  std::shared_ptr<internal::RequestImpl> pimpl_;
};

class ResponseWriter {
  ;

public:
  explicit ResponseWriter(std::shared_ptr<internal::ResponseWriterImpl> pimpl);

  void SetContentType(const std::string &content_type);
  void Write(std::vector<uint8_t> &body);
  ~ResponseWriter();

private:
  std::shared_ptr<internal::ResponseWriterImpl> pimpl_;
};

struct Route {
  typedef std::shared_ptr<Route> Ptr;
  virtual std::string Path() = 0;
  virtual void Handle(Request req, ResponseWriter rw) = 0;
  virtual ~Route() = default;
};

class Server {
public:
  Server();
  ~Server();

  asio::awaitable<void> Start();
  void RegisterRoute(Route::Ptr route);

private:
  std::unique_ptr<internal::ServerImpl> pimpl;
};

// Forward template declarations
template <> int Request::Query(const std::string &key);
template <> std::string Request::Query(const std::string &key);
template <> float Request::Query(const std::string &key);
} // namespace v4s
#endif // !V4L2_STREAM_HTTP_SERVER_H
