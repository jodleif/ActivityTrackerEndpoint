#pragma once
// forward decl
#include <cstdint>
#include <string>
class QByteArray;
class QString;
//

namespace rest {
QByteArray process_request(std::string url, const QByteArray& request_body,
                           QString& mime_type);
}
