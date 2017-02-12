#include <QByteArray>
#include <QDebug>
#include <QJsonDocument>
#include <algorithm>
#include <cassert>
#include <qjsonobject.h>
// own
#include "rest.h"
#include "rest_helpers.h"
#include "strhashliteral/hashliteral.h"

using namespace hash::literals;
namespace canned_response {
constexpr char invalid_json[] = "Invalid JSON\n";
constexpr char invalid_url[] = "Invalid URL\n";
constexpr char invalid_request[] = "Invalid request\n Your request needs to "
                                   "specify username , type of request and "
                                   "password on string format(type,user,pw)\n";
constexpr char invalid_request_type[] = "Invalid request type\n";
}
namespace {
static const std::array<const QString, 3ull> required_for_commit{ "type",
                                                                  "user",
                                                                  "pw" };
static const std::array<const QString, 2ull> strings_for_commit{
  required_for_commit[1], required_for_commit[2]
};
static const std::array<const QString, 1ull> numbers_for_commit{
  required_for_commit[0]
};

QJsonDocument
parse_json(const QByteArray& request_body, QJsonParseError* error)
{
  auto json = QJsonDocument::fromJson(request_body, error);
  return json;
}

bool
is_valid_json(QJsonParseError error)
{
  if (error.error == QJsonParseError::NoError)
    return true;
  qDebug() << "Parsing error: " << error.errorString();
  return false;
}

QJsonDocument
get_json(const QByteArray& request_body, QByteArray& out)
{
  QJsonParseError error;
  auto json = parse_json(request_body, &error);
  if (!(is_valid_json(error))) {
    out = QByteArray(canned_response::invalid_json);
    out.append(error.errorString());
    out.append("\n");
  }
  qDebug() << "Parsed JSON string: "
           << QString(json.toJson(QJsonDocument::Indented));
  return json;
}
bool
verify_commit_request(QJsonDocument json)
{
  auto object = json.object();
  auto keylist = object.keys();
  auto nof_matches = rest::nof_matching_keywords(keylist, required_for_commit);
  // verify correct datatypes for wanted params
  auto kw_strings = rest::specified_kws_are_strings(object, strings_for_commit);
  auto kw_ints = rest::specified_kws_are_numbers(object, numbers_for_commit);
  return (nof_matches == required_for_commit.size()) ? kw_strings && kw_ints
                                                     : false;
}

QByteArray
commit_endpoint(QJsonDocument json)
{
  if (!verify_commit_request(json))
    return QByteArray(canned_response::invalid_request);
  // after basic verification we know that we have "user", "pw" and "type"
  auto json_object = json.object();
  auto req_type = rest::parse_request_type(json_object.take("type").toInt(-1));
  switch (req_type) {
    case rest::request_type::CREATE_USER:
      break;
    case rest::request_type::INVALID:
      return QByteArray(canned_response::invalid_request_type);
      break;
  }

  return QByteArray();
}

QByteArray
request_endpoint(QJsonDocument json)
{

  return QByteArray();
}
}

QByteArray
rest::process_request(std::string url, const QByteArray& request_body)
{
  QByteArray res_body;
  switch (hash::fnv_64_hash(url.c_str())) {
    case "/commit"_f64: {
      auto json = get_json(request_body, res_body);
      if (res_body.size() != 0)
        return res_body;
      return commit_endpoint(json);
    }
    case "/request"_f64: {
      auto json = get_json(request_body, res_body);
      if (res_body.size() != 0)
        return res_body;
      return request_endpoint(json);
    }
    default:
      res_body = QByteArray(canned_response::invalid_url);
      break;
  }
  return res_body;
}

rest::request_type
rest::parse_request_type(int rt)
{
  if (rt > rest::request_type_max || rt < 0)
    return rest::request_type::INVALID;
  return static_cast<rest::request_type>(rt);
}
