#include "utils.h"

namespace tgbotxx {

std::string Utils::ToString(const auto& printable) {
  std::stringstream sstream;
  sstream << printable;
  return sstream.str();
}

std::string Utils::ToString(const Poco::Net::HTTPRequest& request) {
  std::stringstream stream;

  stream
    << std::endl
    << "Request Headers:"
    << std::endl;

  stream << request.getMethod() << " " << request.getURI() << " " << request.getVersion() << std::endl;

  for (const auto& [header, value] : request) {
    stream << header << ": " << value << std::endl;
  }

  return stream.str();
}

std::string Utils::ToString(const Poco::Net::HTTPResponse& response) {
  std::stringstream stream;

  stream
    << std::endl
    << "Response Headers:"
    << std::endl;

  stream << response.getVersion() << " " << response.getStatus() << " " << response.getReason() << std::endl;

  for (const auto& [header, value] : response) {
    stream << header << ": " << value << std::endl;
  }

  return stream.str();
}

void Utils::CutProtocolFromUri(std::string& uri) {
  using namespace std::string_view_literals;

  constexpr auto kProtocolSign = "://"sv;

  const auto protocol_sign_pos = uri.find(kProtocolSign);

  if (protocol_sign_pos == std::string::npos) {
    return;
  }

  const auto indent_size = protocol_sign_pos + kProtocolSign.length();

  for (size_t i = indent_size, j = 0; i < uri.length(); ++i, ++j) {
    uri[j] = uri[i];
  }

  uri.erase(uri.length() - indent_size);
}

}// namespace tgbotxx
