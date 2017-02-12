#pragma once
namespace db {
/**
 * @brief The db_result enum
 * To signal results from db-related functions
 */
enum class db_result_code : int
{
  OK,
  USER_EXISTS,
  USER_DOES_NOT_EXIST,
  WRONG_PASSWORD,
  NO_DB_CONNECTION,
  FAILURE
};
namespace canned_response {
// db_result_code
constexpr static char const* db_ok = "Request completed OK!\n";
constexpr static char const* db_failure = "Requst to DB failed!\n";
constexpr static char const* no_db_connection = "No connection to Database!\n";
constexpr static char const* user_does_not_exist = "User does not exist!\n";
constexpr static char const* user_exists =
  "User already present in database!\n";
constexpr static char const* wrong_password = "Wrong password!\n";
}
}
namespace rest {
namespace canned_response {
constexpr static char const* invalid_json = "Invalid JSON\n";
constexpr static char const* invalid_url = "Invalid URL\n";
constexpr static char const* invalid_request =
  "Invalid request\n Your request needs to "
  "specify username , type of request and "
  "password on string format(type,user,pw)\n";
constexpr static char const* invalid_request_type = "Invalid request type\n";
}
enum class response
{
  INVALID_JSON,
  INVALID_URL,
  INVALID_REQUEST,
  INVALID_REQUEST_TYPE
};

constexpr char const*
response(response resp)
{
  switch (resp) {
    case response::INVALID_JSON:
      return canned_response::invalid_json;
    case response::INVALID_REQUEST:
      return canned_response::invalid_request;
    case response::INVALID_REQUEST_TYPE:
      return canned_response::invalid_request_type;
    case response::INVALID_URL:
      return canned_response::invalid_url;
  }
  return "";
}
constexpr char const*
response(db::db_result_code code)
{
  using r = db::db_result_code;
  switch (code) {
    case r::OK:
      return db::canned_response::db_ok;
    case r::FAILURE:
      return db::canned_response::db_failure;
    case r::NO_DB_CONNECTION:
      return db::canned_response::no_db_connection;
    case r::USER_DOES_NOT_EXIST:
      return db::canned_response::user_does_not_exist;
    case r::USER_EXISTS:
      return db::canned_response::user_exists;
    case r::WRONG_PASSWORD:
      return db::canned_response::wrong_password;
  }
  return "";
}
}
