#include "postgres.h"
#include <iostream>
#include <cassert>

void db::make_transaction(pqxx::connection * ptr,std::string sql_query)
{
    assert(ptr);
    pqxx::work transaction(*ptr);
    auto r = transaction.exec("select * from activity");
    if(r.size() == 0) return;
    std::cout << "Got " << r.size() << " results\n";
    for(const auto& row : r){
        for(const auto& field : row){
            std::cout << field.name() << ":" << field.c_str() << "\t";
        }
        std::cout << '\n';
    }

}

/**
 * @brief db::open_db_connection
 * Will open a new database-connection based on the following shell-variables
 *  PGDATABASE  (name of database; defaults to your user name)
 *  PGHOST      (database server; defaults to local machine)
 *  PGPORT      (TCP port to connect to; default is 5432)
 *  PGUSER      (your PostgreSQL user ID; defaults to your login name)
 *  PGPASSWORD  (your PostgreSQL password, if needed)
 * @return unique_ptr with db-handle
 */
std::unique_ptr<pqxx::connection> db::open_db_connection()
{
    try {
    return std::unique_ptr<pqxx::connection>(new pqxx::connection());
    } catch (std::exception & e) {
        std::cerr << "Failed connecting to DB\n"
                  << e.what()
                  << std::endl;
    }
    return std::unique_ptr<pqxx::connection>(nullptr);
}
