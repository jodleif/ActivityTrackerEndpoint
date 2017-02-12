#pragma once
class QByteArray;
class QJsonDocument;
namespace rest {

enum class request_type : int
{
  CREATE_USER = 0,
  INSERT_EVENT = 1,
  INVALID = -1
};
constexpr static int request_type_max = 1;

request_type parse_request_type(int rt);
QByteArray commit_endpoint(QJsonDocument json);

QByteArray request_endpoint(QJsonDocument json);
}
