#include "request_handling.h"
#include "responses.h"
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>

QByteArray
rest::commit_endpoint(QJsonDocument json)
{
  // after basic verification we know that we have "user", "pw" and "type"
  auto json_object = json.object();
  auto req_type = rest::parse_request_type(json_object.take("type").toInt(-1));
  switch (req_type) {
    case rest::request_type::CREATE_USER:
      break;
    case rest::request_type::INSERT_EVENT:
      break;
    case rest::request_type::INVALID:
      return QByteArray(response(response::INVALID_REQUEST_TYPE));
  }

  return QByteArray();
}

QByteArray
rest::request_endpoint(QJsonDocument json)
{
  return QByteArray();
}

rest::request_type
rest::parse_request_type(int rt)
{
  if (rt > rest::request_type_max || rt < 0)
    return rest::request_type::INVALID;
  return static_cast<rest::request_type>(rt);
}
