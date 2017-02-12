#pragma once
// forward decl
#include <cstdint>
#include <string>
class QByteArray;
//

namespace rest {
QByteArray process_request(std::string url, const QByteArray& request_body);
}
