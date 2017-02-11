#pragma once
//forward decl
#include <string>
class QByteArray;
//
namespace rest {
QByteArray process_request(std::string url);
}
