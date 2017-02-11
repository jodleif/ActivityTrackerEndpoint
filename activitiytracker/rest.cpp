#include "rest.h"
#include "strhashliteral/hashliteral.h"
#include <QByteArray>
#include <QJsonDocument>
#include <cassert>

using namespace hash::literals;
namespace canned_response {
constexpr char invalid_json[] = "Invalid JSON\n";
}
namespace {
std::pair<QJsonDocument,QJsonParseError> parse_json(const QByteArray & request_body)
{
    QJsonParseError error;
    auto json = QJsonDocument::fromJson(request_body,&error);
    return {json,error};
}

bool is_valid_json(QJsonParseError error)
{
    if(error == QJsonParseError::NoError) return true;
    return false;
}

QByteArray commit_endpoint(const QByteArray& request_body)
{
    auto json_pair = parse_json(request_body);

}

QByteArray request_endpoint(const QByteArray& request_body)
{

}
}

QByteArray rest::process_request(std::string url, const QByteArray& request_body)
{
    QByteArray res_body;
    switch(hash::fnv_64_hash(url.c_str())) {
    case "/commit"_f64:
        return commit_endpoint(request_body);
    case "/request"_f64:
        return request_endpoint(request_body);
    default:
        res_body = "Invalid URL\n";
        break;
    }
    return res_body;
}
