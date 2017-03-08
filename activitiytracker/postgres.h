#pragma once
#include "activity_event.h"
#include <experimental/optional>
#include <iostream>
#include <memory>
#include <pqxx/pqxx>
template <class T>
using optional = std::experimental::optional<T>;

using unique_db_ptr =
  std::unique_ptr<pqxx::connection, std::function<void(pqxx::connection*)>>;

namespace db {
// defined in response.h
enum class db_result_code;

/**
 * Wrapper fn to run queries in.
 * Prints errors to stderr
 * @returns optional result.
 */
template <class T>
optional<pqxx::result>
db_try_block(T&& fn, std::string error_msg)
{
  try {
    return std::experimental::make_optional(fn());
  } catch (std::exception& e) {
    std::cerr << error_msg << e.what() << std::endl;
  }
  return {}; // empty optional
}

bool execute_query(pqxx::connection* ptr, std::string sql_query,
                   std::vector<std::vector<std::string>>& query_result);
db_result_code insert_user(pqxx::connection* ptr, std::string email,
                           std::string password);
db_result_code delete_user(pqxx::connection* ptr, std::string email,
                           std::string password);
bool prepare_connection(pqxx::connection* ptr);
db_result_code check_login(pqxx::connection* ptr, std::string email,
                           std::string password);
db_result_code insert_events(pqxx::connection* ptr, std::string email,
                             std::vector<event::activity_event>);
optional<std::vector<std::vector<std::string>>> dump_db();
unique_db_ptr open_db_connection();
}
