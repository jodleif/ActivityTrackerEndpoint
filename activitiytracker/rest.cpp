#include <QByteArray>
#include <QDebug>
#include <QJsonDocument>
#include <QString>
#include <algorithm>
#include <cassert>
#include <qjsonobject.h>
// own
#include "request_handling.h"
#include "responses.h"
#include "rest.h"
#include "rest_helpers.h"
#include "strhashliteral/hashliteral.h"

using namespace hash::literals;
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
    out = QByteArray(rest::response_string(rest::response_t::INVALID_JSON));
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
}

QByteArray
rest::process_request(std::string url, const QByteArray& request_body,
                      QString& mime_type)
{
  QByteArray res_body;
  switch (hash::fnv_64_hash(url.c_str())) {
    case "/commit"_f64: {
      auto json = get_json(request_body, res_body);
      if (res_body.size() != 0)
        return res_body;

      if (!verify_commit_request(json))
        return QByteArray(
          rest::response_string(rest::response_t::INVALID_REQUEST));
      return rest::commit_endpoint(json);
    }
    case "/request"_f64: {
      auto json = get_json(request_body, res_body);
      if (res_body.size() != 0)
        return res_body;
      return rest::request_endpoint(json);
    }
    case "/db_dump"_f64: {
      mime_type = "application/x-xls";
      return rest::db_dump();
    }
    default:
      res_body =
        QByteArray(rest::response_string(rest::response_t::INVALID_URL));
      break;
  }
  return res_body;
}
