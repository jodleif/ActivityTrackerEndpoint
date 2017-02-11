#ifndef POSTGRES_H
#define POSTGRES_H
#include <experimental/optional>
#include <iostream>
#include <memory>
#include <pqxx/pqxx>
namespace db {

template <class T>
using optional = std::experimental::optional<T>;

enum class db_result : int
{
  OK,
  USER_EXISTS,
  USER_DOES_NOT_EXIST,
  WRONG_PASSWORD,
  FAILURE
};
template <class T>
optional<pqxx::result>
db_try_block(T&& fn, std::string error_msg)
{
  try {
    return optional<pqxx::result>(fn());
  } catch (std::exception& e) {
    std::cerr << error_msg << e.what() << std::endl;
  }
  return optional<pqxx::result>();
}

bool make_transaction(pqxx::connection* ptr, std::string sql_query);
db_result insert_user(pqxx::connection* ptr, std::string email,
                      std::string password);
db_result delete_user(pqxx::connection* ptr, std::string email,
                      std::string password);
bool prepare_connection(pqxx::connection* ptr);
std::unique_ptr<pqxx::connection, std::function<void(pqxx::connection*)>>
open_db_connection();
}
#endif // POSTGRES_H
