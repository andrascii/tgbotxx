#pragma once

namespace tgbotxx {

class Utils {
 public:
  //!
  //! Transforms passed argument to the string if it printable. (TODO: add constraint).
  //!
  static std::string ToString(const auto& printable);
  static std::string ToString(const Poco::Net::HTTPRequest& request);
  static std::string ToString(const Poco::Net::HTTPResponse& response);

  //!
  //! Received URIs and cuts protocol part.
  //! E.g. - https://example.org ==> example.org
  //!
  static void CutProtocolFromUri(std::string& uri);
};

}// namespace tgbotxx
