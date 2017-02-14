#include "db_helpers.h"
#include "postgres.h"
#include "responses.h"
#include <cassert>
#include <iostream>

namespace db {
namespace sql_statements {
constexpr char insert_event[] =
  "INSERT INTO user_events (userid,activity_id,event_time)"
  "SELECT users.userid,$2,to_timestamp($3) FROM users WHERE users.email=$1";
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
    auto pw_check = check_login(ptr, email, password);
    if (pw_check != db_result_code::OK)
      return pw_check;
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

db_result_code
check_login(pqxx::connection* ptr, std::string email, std::string password)
{
  if (!db::verify_connection(ptr))
    return db_result_code::NO_DB_CONNECTION;
  pqxx::work transaction(*ptr);
  auto res = db::db_try_block(
    [email, password, &transaction]() {
      return transaction.prepared("check_password")(email)(password).exec();
    },
    "DB fail when check_login");
  if (!res)
    return db_result_code::FAILURE;
  if ((*res).size() >= 1)
    return db_result_code::OK;
  return db_result_code::WRONG_PASSWORD;
}
/**
 * @brief insert_events
 * Assumes email is valid
 * @param db_conn connection to database
 * @param email users email (verified)
 * @return operation succesful?
 */
db_result_code
insert_events(pqxx::connection* db_conn, std::string email,
              std::vector<event::activity_event> events)
{
  if (!db::verify_connection(db_conn))
    return db_result_code::NO_DB_CONNECTION;
  {
    pqxx::work transaction(*db_conn);
    auto res = db_try_block(
      [email, &events, &transaction]() {
        for (const auto& event : events) {
          transaction
            .prepared("insert_event")(email)(event.activity)(event.timestamp)
            .exec();
        }
        // If no exception has been thrown - commit transaction
        transaction.commit();
        return pqxx::result(); // placeholder object to signal correct result
      },
      "Failed to insert events");
    if (res) {
      return db_result_code::OK;
    }
  }

  return db_result_code::FAILURE;
}
}
