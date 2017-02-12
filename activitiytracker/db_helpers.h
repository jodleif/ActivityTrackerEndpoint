#pragma once
/**
 * Assumes connection ptr has been created with
 * helpers in postgres.cpp
 */
#include <pqxx/pqxx>
namespace db {
bool verify_connection(pqxx::connection* ptr);
}
