/*!
 * \file table_factory.cc
 * \brief The table factory
 */
#include "hpps/frame/table_factory.h"

namespace hpps {

namespace table_factory {

std::vector<ServerTable*> server_tables;

void FreeServerTables() {
  for (auto table : server_tables) {
    delete table;
  }
  server_tables.clear();
}

void PushServerTable(ServerTable*table) {
  server_tables.push_back(table);
}

} // namespace table_factory

} // namespace hpps
