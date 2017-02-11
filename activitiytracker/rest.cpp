#include "rest.h"
#include "strhashliteral/hashliteral.h"
#include <QByteArray>
#include <QDebug>
#include <QJsonDocument>
#include <cassert>

using namespace hash::literals;
namespace canned_response {
constexpr char invalid_json[] = "Invalid JSON\n";
constexpr char invalid_url[] = "Invalid URL\n";
}
namespace {
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

QByteArray
commit_endpoint(QJsonDocument json)
{

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
