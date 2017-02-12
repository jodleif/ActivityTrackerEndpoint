#pragma once

namespace rest {
namespace canned_response {
constexpr static char const* invalid_json = "Invalid JSON\n";
constexpr static char const* invalid_url = "Invalid URL\n";
constexpr static char const* invalid_request =
  "Invalid request\n Your request needs to "
  "specify username , type of request and "
  "password on string format(type,user,pw)\n";
constexpr static char const* invalid_request_type = "Invalid request type\n";
}
enum class response
{
  INVALID_JSON,
  INVALID_URL,
  INVALID_REQUEST,
  INVALID_REQUEST_TYPE
};

constexpr char const*
response(response resp)
{
  switch (resp) {
    case response::INVALID_JSON:
      return canned_response::invalid_json;
    case response::INVALID_REQUEST:
      return canned_response::invalid_request;
    case response::INVALID_REQUEST_TYPE:
      return canned_response::invalid_request_type;
    case response::INVALID_URL:
      return canned_response::invalid_url;
  }
  return "";
}
}
