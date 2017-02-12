#include "postgres.h"
#include <cassert>
#include <iostream>

namespace db {
namespace sql_statements {
constexpr char insert_event[] =
  "INSERT INTO user_events VALUES (default,$1,$2,$3)";
constexpr char check_password[] =
  "SELECT * FROM users WHERE email=$1 AND password=$2";
constexpr char insert_user[] =
  "INSERT INTO users VALUES (default,$1,$2,current_date)";
constexpr char delete_user[] = "DELETE FROM users WHERE email=$1";
constexpr char show_activities[] = "SELECT * FROM activity";
constexpr char find_user[] = "SELECT userid FROM users where email=$1";
} // end namespace sql_statements
namespace {

int
does_user_exist(pqxx::connection* ptr, std::string email)
{
  pqxx::work transaction(*ptr);
  auto res = db_try_block(
    [email, &transaction]() -> pqxx::result {
      return transaction.prepared("find_user")(email).exec();
    },
    "Failed to execute find_user\n");
  if (!res)
    return 0;
  std::cout << "Got " << (*res).size() << " results";
  if ((*res).size() == 0)
    return 0;
  return (*res)[0][0].as<int>();
}
} // end anon ns
bool
prepare_connection(pqxx::connection* ptr)
{
  assert(ptr);
  ptr->prepare("insert_event", sql_statements::insert_event);
  ptr->prepare("check_password", sql_statements::check_password);
  ptr->prepare("insert_user", sql_statements::insert_user);
  ptr->prepare("show_activity", sql_statements::show_activities);
  ptr->prepare("find_user", sql_statements::find_user);
  ptr->prepare("delete_user", sql_statements::delete_user);
  return true;
}

db_result_code
insert_user(pqxx::connection* ptr, std::string email, std::string password)
{
  assert(ptr);
  auto userid = does_user_exist(ptr, email);
  if (userid) {
    std::cerr << "User exists with id:" << userid << std::endl;
    return db_result_code::USER_EXISTS;
  }

  pqxx::work transaction(*ptr);
  auto res = db_try_block(
    [email, password, &transaction]() {
      auto res = transaction.prepared("insert_user")(email)(password).exec();
      transaction.commit();
      return res;
    },
    "Failed inserting user\n");

  if (!res)
    return db_result_code::FAILURE;

  return db_result_code::OK;
}
db_result_code
delete_user(pqxx::connection* ptr, std::string email, std::string password)
{
  {
    pqxx::work transaction(*ptr);
    auto res = db_try_block(
      [email, password, &transaction]() {
        return transaction.prepared("check_password")(email)(password).exec();
      },
      "Failed to query for matching password");
    if (!res)
      return db_result_code::FAILURE;
    if ((*res).size() == 0)
      return db_result_code::WRONG_PASSWORD;
  }
  pqxx::work transaction(*ptr);
  auto res = db_try_block(
    [email, &transaction]() {
      auto res = transaction.prepared("delete_user")(email).exec();
      transaction.commit();
      return res;
    },
    "Failed query to delete password");

  if (!res)
    return db_result_code::FAILURE;
  return db_result_code::OK;
}
}
