#ifndef POSTGRES_H
#define POSTGRES_H
#include <pqxx/pqxx>
#include <memory>
namespace db {
    void make_transaction(pqxx::connection * ptr,std::string sql_query);
    std::unique_ptr<pqxx::connection> open_db_connection();
}
#endif // POSTGRES_H
