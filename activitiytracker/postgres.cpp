#include "postgres.h"
#include "responses.h"
#include <cassert>
#include <functional>
#include <iostream>
namespace {
constexpr char dump_database[] =
  "select event_time, activity.activity, users.email from activity, "
  "user_events, users where activity.activity_id=user_events.activity_id AND "
  "user_events.userid=users.userid";
}
using conn_dtor_fn = std::function<void(pqxx::connection*)>;
using db_unq_ptr = std::unique_ptr<pqxx::connection, conn_dtor_fn>;
bool
db::execute_query(pqxx::connection* ptr, std::string sql_query,
                  std::vector<std::vector<std::string>>& data)
{
  assert(ptr);
  assert(sql_query.size() > 0);
  auto qres = db::db_try_block(
    [ptr, &sql_query, &data]() {
      pqxx::work transaction(*ptr);
      auto r = transaction.exec(sql_query);
      data.reserve(r.size());
      for (const auto& row : r) {
        std::vector<std::string> data_row;
        data_row.reserve(row.size());
        for (pqxx::field cell : row) {
          data_row.emplace_back(cell.c_str());
        }
        data.emplace_back(std::move(data_row));
      }
      return r;
    },
    "Error during SQL query");
  if (qres)
    return true;
  return false;
}

/**
 * @brief db::open_db_connection
 * Will open a new database-connection based on the following env-variables
 *  PGDATABASE  (name of database; defaults to your user name)
 *  PGHOST      (database server; defaults to local machine)
 *  PGPORT      (TCP port to connect to; default is 5432)
 *  PGUSER      (your PostgreSQL user ID; defaults to your login name)
 *  PGPASSWORD  (your PostgreSQL password, if needed)
 * Will also initialize the connection with the prepared statements defined in
 * postgres_prepared_statments.cpp
 * @return unique_ptr with db-handle
 */
db_unq_ptr
db::open_db_connection()
{
  // initialize connection to database - if an exception get's thrown
  // log to stderr and return a nullptr
  auto db_ptr = []() -> db_unq_ptr {
    try {
      return db_unq_ptr(new pqxx::connection(),
                        [&](pqxx::connection* connection) {
                          connection->disconnect();
                          delete connection;
                        });
    } catch (std::exception& e) {
      std::cerr << "Failed connecting to DB\n" << e.what() << std::endl;
    }
    return { nullptr };
  }();
  // if db_connection opened, also initialize prepared statements in connection
  if (db_ptr) {
    prepare_connection(db_ptr.get());
  }
  return db_ptr;
}

optional<std::vector<std::vector<std::string>>>
db::dump_db()
{
  auto db_res = db::open_db_connection();
  std::vector<std::vector<std::string>> data;
  data.emplace_back(std::vector<std::string>(
    { { "Header 1" }, { "Header 2" }, { "Header 3" } }));
  auto success =
    db::execute_query(db_res.get(), std::string(dump_database), std::ref(data));
  if (success)
    return std::experimental::make_optional(data);
  return {};
}
