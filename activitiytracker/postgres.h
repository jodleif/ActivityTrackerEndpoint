#ifndef POSTGRES_H
#define POSTGRES_H
#include <memory>
#include <pqxx/pqxx>
namespace db {
bool make_transaction(pqxx::connection* ptr, std::string sql_query);

std::unique_ptr<pqxx::connection, std::function<void(pqxx::connection*)>> open_db_connection();
}
#endif // POSTGRES_H
