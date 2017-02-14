#include <QByteArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <experimental/optional>
#include <utility>
#include <vector>

#include "activity_event.h"
#include "postgres.h"
#include "request_handling.h"
#include "responses.h"
#include "rest_helpers.h"

// usings
using user_and_email =
  std::experimental::optional<std::pair<std::string, std::string>>;
using events = std::experimental::optional<std::vector<event::activity_event>>;
using opt_event = std::experimental::optional<event::activity_event>;

namespace {
const static std::array<const QString, 2> required_for_event{
  { QString("act"), QString("ts") }
};
const static std::array<const QString, 1> must_be_arrays{ { "events" } };
const static std::array<const QString, 1> must_be_strings{ { QString("ts") } };
const static std::array<const QString, 1> must_be_doubles{ { QString("act") } };
user_and_email
fetch_and_verify_user_and_email(QJsonObject json)
{
  auto pw = json.value("pw").toString("");
  auto user = json.value("user").toString("");
  if (pw.size() == 0 || user.size() == 0)
    return {};
  std::string username{ user.toStdString() };
  std::string password{ pw.toStdString() };
  return std::experimental::make_optional(std::make_pair(username, password));
}

QByteArray
create_user(pqxx::connection* db_connection, std::string username,
            std::string password)
{
  auto db_res = db::insert_user(db_connection, username, password);
  return QByteArray(rest::response(db_res));
}

bool
verify_event(const QJsonObject& json_object)
{
  bool has_timestamp =
    rest::specified_kws_are_strings(json_object, must_be_strings);
  bool has_activity =
    rest::specified_kws_are_numbers(json_object, must_be_doubles);
  return has_activity && has_timestamp;
}

opt_event
parse_event(const QJsonObject& json_object)
{
  event::activity_event ev;

  if (!verify_event(json_object))
    return {};

  QString timestamp = json_object.value("ts").toString("a");
  bool string_parse{ false };
  // todo: proper validation of activity
  ev.activity = json_object.value("act").toInt(-1);
  ev.timestamp = static_cast<int64_t>(timestamp.toLongLong(&string_parse, 10));
  if (string_parse) // parse to long longok?
  {
    qDebug() << "ACtivity" << ev.activity << " Timestamp" << ev.timestamp;
    return std::experimental::make_optional(ev);
  }
  return {};
}

events
parse_events(const QJsonObject& json_object)
{
  if (!rest::specified_kws_are_arrays(json_object, must_be_arrays))
    return {};
  events ev;
  ev.emplace(std::vector<event::activity_event>());
  auto& vector_ref = (*ev);
  QJsonArray json_array = json_object.value("events").toArray();
  qDebug() << "Json_array length" << json_array.size();
  for (const QJsonValueRef event : json_array) {
    if (event.isObject()) {
      auto return_val = parse_event(event.toObject());
      if (return_val) {
        vector_ref.emplace_back(std::move(*return_val));
      } else
        return {}; // empty optional
    }
  }
  return ev;
}

QByteArray
insert_events(pqxx::connection* db_connection, std::string username,
              std::string password, const events& event)
{
  // to insert events you need a valid login
  auto is_valid_login = db::check_login(db_connection, username, password);
  if (is_valid_login != db::db_result_code::OK)
    return QByteArray(rest::response(is_valid_login));

  auto unboxed_events = *event; // extract from optional
  auto result = db::insert_events(db_connection, username, unboxed_events);
  return QByteArray(rest::response(result));
}
}

QByteArray
rest::commit_endpoint(QJsonDocument json)
{

  static int i{ 1 };
  qDebug() << "Commit endpoint call: " << i++;

  // TODO: Proper connection management
  auto db_connection = db::open_db_connection();
  // after basic verification we know that we have "user", "pw" and "type"
  auto json_object = json.object();
  auto username_and_password = fetch_and_verify_user_and_email(json_object);
  if (!username_and_password)
    return QByteArray(response(db::db_result_code::WRONG_PASSWORD));
  auto username = (*username_and_password).first;
  auto password = (*username_and_password).second;

  auto req_type = rest::parse_request_type(json_object.take("type").toInt(-1));
  qDebug() << "Passed type verification";
  switch (req_type) {
    case rest::request_type::CREATE_USER: {
      auto res = create_user(db_connection.get(), username, password);
      return res;
    }
    case rest::request_type::INSERT_EVENT: {
      auto event_vector = parse_events(json_object);
      if (event_vector) {
        auto result =
          insert_events(db_connection.get(), username, password, event_vector);
        return result;
      }
      break;
    }
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
