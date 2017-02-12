#pragma once
// forward decl
#include <cstdint>
#include <string>
class QByteArray;
//

namespace rest {
enum class request_type : int
{
  CREATE_USER = 0,
  INVALID = -1
};
constexpr int request_type_max = 0;

request_type parse_request_type(int rt);
QByteArray process_request(std::string url, const QByteArray& request_body);
}
