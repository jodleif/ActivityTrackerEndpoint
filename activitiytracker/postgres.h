#pragma once
#include <experimental/optional>
#include <iostream>
#include <memory>
#include <pqxx/pqxx>

template <class T>
using optional = std::experimental::optional<T>;

using unique_db_ptr =
  std::unique_ptr<pqxx::connection, std::function<void(pqxx::connection*)>>;
namespace db {

/**
 * @brief The db_result enum
 * To signal results from db-related functions
 */
enum class db_result : int
{
  OK,
  USER_EXISTS,
  USER_DOES_NOT_EXIST,
  WRONG_PASSWORD,
  FAILURE
};

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

bool execute_query(pqxx::connection* ptr, std::string sql_query);
db_result insert_user(pqxx::connection* ptr, std::string email,
                      std::string password);
db_result delete_user(pqxx::connection* ptr, std::string email,
                      std::string password);
bool prepare_connection(pqxx::connection* ptr);
unique_db_ptr open_db_connection();
}
