#pragma once

namespace tgbotxx {

class SyncHttpClient final {
 public:
  explicit SyncHttpClient(const std::string& host);

  std::string Get(const std::string& path);
  std::string Post(const std::string& path, const std::string& body);

 private:
  Poco::Net::HTTPRequest CreateRequest(
    const std::string& method,
    const std::string& path) const;

  void RecreateSession(const Poco::URI& uri);

 private:
  std::unique_ptr<Poco::Net::HTTPClientSession> session_;
};

}// namespace tgbotxx
