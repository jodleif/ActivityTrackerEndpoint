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
