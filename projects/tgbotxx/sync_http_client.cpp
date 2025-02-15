#include "sync_http_client.h"

#include "logger.h"
#include "utils.h"

namespace {

[[maybe_unused]] constexpr const char* kUserAgent = "tgbotxx";

}

namespace tgbotxx {

SyncHttpClient::SyncHttpClient(const std::string& host) {
  RecreateSession(Poco::URI{host});
}

std::string SyncHttpClient::Get(const std::string& path) {
  auto request = CreateRequest(Poco::Net::HTTPRequest::HTTP_GET, path);
  session_->sendRequest(request);

  Poco::Net::HTTPResponse response;
  auto response_stream = std::ref(session_->receiveResponse(response));

  LOG_TRACE(Utils::ToString(request));
  LOG_TRACE(Utils::ToString(response));

  while (response.getStatus() == Poco::Net::HTTPResponse::HTTP_MOVED_PERMANENTLY ||
         response.getStatus() == Poco::Net::HTTPResponse::HTTP_FOUND) {
    const auto location = response.get("Location");
    const auto uri = Poco::URI{location};

    RecreateSession(uri);
    const auto path = uri.getPathAndQuery();

    request.setURI(path);
    request.setContentLength(0);

    session_->sendRequest(request);
    response.clear();

    response_stream = std::ref(session_->receiveResponse(response));

    LOG_TRACE(Utils::ToString(request));
    LOG_TRACE(Utils::ToString(response));
  }

  std::string response_body;
  Poco::StreamCopier::copyToString(response_stream, response_body);

  return response_body;
}

std::string SyncHttpClient::Post(
  const std::string& path,
  const std::string& body) {
  auto request = CreateRequest(Poco::Net::HTTPRequest::HTTP_POST, path);
  request.setContentType("application/x-www-form-urlencoded");
  request.setContentLength(body.length());

  auto body_stream = std::ref(session_->sendRequest(request));
  body_stream.get() << body;

  Poco::Net::HTTPResponse response;
  auto response_stream = std::ref(session_->receiveResponse(response));

  while (response.getStatus() == Poco::Net::HTTPResponse::HTTP_MOVED_PERMANENTLY ||
         response.getStatus() == Poco::Net::HTTPResponse::HTTP_FOUND) {
    const auto location = response.get("Location");
    const auto uri = Poco::URI{location};

    RecreateSession(uri);
    const auto path = uri.getPathAndQuery();

    request.setURI(path);
    request.setContentLength(body.length());

    body_stream = session_->sendRequest(request);
    body_stream.get() << body;

    response.clear();
    response_stream = std::ref(session_->receiveResponse(response));
  }

  std::string response_body;
  Poco::StreamCopier::copyToString(response_stream, response_body);

  return response_body;
}

Poco::Net::HTTPRequest
SyncHttpClient::CreateRequest(
  const std::string& method,
  const std::string& path) const {
  const auto uri = Poco::URI{path};

  auto request = Poco::Net::HTTPRequest{
    method,
    uri.getPathAndQuery(),
    Poco::Net::HTTPMessage::HTTP_1_1};

  request.setHost(session_->getHost());
  request.set("Accept", "*/*");
  request.set("User-Agent", kUserAgent);
  request.set("Accept-Encoding", "identity");
  request.setContentType("application/x-www-form-urlencoded");
  request.setKeepAlive(true);

  return request;
}

void SyncHttpClient::RecreateSession(const Poco::URI& uri) {
  if (uri.getScheme() == "https") {
    session_ = std::make_unique<Poco::Net::HTTPSClientSession>(uri.getHost(), uri.getPort());
    return;
  }

  session_ = std::make_unique<Poco::Net::HTTPClientSession>(uri.getHost(), uri.getPort());
}

}// namespace tgbotxx
