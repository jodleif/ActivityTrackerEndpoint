#include "db_helpers.h"
#include <iostream>

bool
db::verify_connection(pqxx::connection* ptr)
{
  bool db_status{ false };
  if (ptr == nullptr) {
    db_status = false;
  } else {
    db_status = ptr->is_open();
  }
  if (!db_status) {
    std::cerr << "DB connection not active!";
  }
  return db_status;
}
