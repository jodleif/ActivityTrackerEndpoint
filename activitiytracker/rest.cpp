#include "rest.h"
#include "strhashliteral/hashliteral.h"
#include <QByteArray>
#include <QDebug>
#include <QJsonDocument>
#include <cassert>
#include <qjsonobject.h>

using namespace hash::literals;
namespace canned_response {
constexpr char invalid_json[] = "Invalid JSON\n";
constexpr char invalid_url[] = "Invalid URL\n";
constexpr char invalid_request[] = "Invalid request\n Your request needs to "
                                   "specify username , type of request and "
                                   "password on string format(type,user,pw)\n";
}
namespace {
static const std::array<const QString, 3ull> required_for_commit{ "type",
                                                                  "user",
                                                                  "pw" };

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
  auto nof_matches = std::accumulate(
    required_for_commit.begin(), required_for_commit.end(), 0u,
    [&keylist](unsigned int prev, const auto& to_match) -> int {
      if (keylist.contains(to_match, Qt::CaseSensitivity::CaseInsensitive)) {
        prev += 1;
      }
      return prev;
    });
  if (nof_matches == required_for_commit.size()) {
    // verify that all are strings
    for (const auto& str : required_for_commit) {
      if (!(object.take(str).isString()))
        return false;
    }
  } else
    return false;
  return true;
}

QByteArray
commit_endpoint(QJsonDocument json)
{
  if (!verify_commit_request(json))
    return QByteArray(canned_response::invalid_request);
  // after basic verification we know that we have "user", "pw" and "type"
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
