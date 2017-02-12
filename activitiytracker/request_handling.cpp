#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <experimental/optional>
#include <utility>

#include "postgres.h"
#include "request_handling.h"
#include "responses.h"

using user_and_email =
  std::experimental::optional<std::pair<std::string, std::string>>;
namespace {
user_and_email
fetch_and_verify_user_and_email(QJsonObject json)
{
  auto pw = json.take("pw").toString("");
  auto user = json.take("user").toString("");
  if (pw.size() == 0 || user.size() == 0)
    return {};
  std::string username{ user.toStdString() };
  std::string password{ pw.toStdString() };
  return std::experimental::make_optional(std::make_pair(username, password));
}
QByteArray
create_user(pqxx::connection* db_connection, QJsonObject json)
{

  auto user_pw{ fetch_and_verify_user_and_email(json) };
  if (user_pw) {
    auto unwrapped = *user_pw;
    auto db_res =
      db::insert_user(db_connection, unwrapped.first, unwrapped.second);
    return QByteArray(rest::response(db_res));
  }
  return QByteArray(rest::response(rest::response::INVALID_JSON));
}
}

QByteArray
rest::commit_endpoint(QJsonDocument json)
{
  // TODO: Proper connection management
  auto db_connection = db::open_db_connection();
  // after basic verification we know that we have "user", "pw" and "type"
  auto json_object = json.object();
  auto req_type = rest::parse_request_type(json_object.take("type").toInt(-1));
  switch (req_type) {
    case rest::request_type::CREATE_USER: {
      auto res = create_user(db_connection.get(), json_object);
      return res;
    }
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
