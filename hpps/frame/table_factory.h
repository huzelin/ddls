/*!
 * \file table_factory.h
 * \brief The table factory
 */
#pragma once

#include "hpps/frame/table_interface.h"
#include "hpps/frame/zoo.h"

#include <string>

namespace hpps {
namespace table_factory {

void FreeServerTables();
void PushServerTable(ServerTable*table);

template <typename OptionType>
typename OptionType::WorkerTableType* CreateTable(const OptionType& option) {
  if (Zoo::Get()->server_rank() >= 0) {
    PushServerTable(
      new typename OptionType::ServerTableType(option));
  }
  if (Zoo::Get()->worker_rank() >= 0) {
    return new typename OptionType::WorkerTableType(option);
  }
  return nullptr;
}

} // namespace table_factory
} // namespace hpps
