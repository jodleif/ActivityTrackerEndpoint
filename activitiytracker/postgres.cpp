#include "postgres.h"
#include "responses.h"
#include <cassert>
#include <functional>
#include <iostream>

using conn_dtor_fn = std::function<void(pqxx::connection*)>;
bool
db::execute_query(pqxx::connection* ptr, std::string sql_query)
{
  assert(ptr);
  assert(sql_query.size() > 0);
  auto qres = db::db_try_block(
    [ptr, &sql_query]() {
      pqxx::work transaction(*ptr);
      auto r = transaction.exec(sql_query);
      std::cout << "Got " << r.size() << " results\n";
      for (const auto& row : r) {
        for (const auto field : row) {
          std::cout << field.name() << ":" << field.c_str() << "\t";
        }
        std::cout << '\n';
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
std::unique_ptr<pqxx::connection, conn_dtor_fn>
db::open_db_connection()
{
  auto db_ptr = []() -> std::unique_ptr<pqxx::connection, conn_dtor_fn> {
    try {
      return std::unique_ptr<pqxx::connection, conn_dtor_fn>(
        new pqxx::connection(), [&](pqxx::connection* connection) {
          connection->disconnect();
          delete connection;
        });
    } catch (std::exception& e) {
      std::cerr << "Failed connecting to DB\n" << e.what() << std::endl;
    }
    return { nullptr };
  }();
  if (db_ptr) {
    prepare_connection(db_ptr.get());
  }
  return db_ptr;
}
