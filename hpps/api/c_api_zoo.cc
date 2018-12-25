/*
 * \file c_api_zoo.cc
 */
#include "hpps/api/c_api_zoo.h"

#include "hpps/api/c_api_error.h"
#include "hpps/common/log.h"
#include "hpps/common/string_util.h"
#include "hpps/frame/zoo.h"

using namespace hpps;

int HPPS_ZooStart(const char* args) {
  auto splits = StrSplit(args, ' ');
  std::vector<char*> data;
  data.resize(splits.size());
  int size = data.size();
  for (auto i = 0; i < data.size(); ++i) {
    data[i] = const_cast<char*>(splits[i].c_str());
  }
  Zoo::Get()->Start(&size, data.data());
  return 0;
}

int HPPS_ZooStop() {
  Zoo::Get()->Stop(true);
  return 0;
}

int HPPS_ZooBarrier() {
  Zoo::Get()->Barrier();
  return 0;
}

int HPPS_ZooRank(int* rank) {
  *rank = Zoo::Get()->rank();
  return 0;
}

int HPPS_ZooSize(int* size) {
  *size = Zoo::Get()->size();
  return 0;
}

int HPPS_ZooNumWorkers(int* num_workers) {
  *num_workers = Zoo::Get()->num_workers();
  return 0;
}

int HPPS_ZooNumServers(int* num_servers) {
  *num_servers = Zoo::Get()->num_servers();
  return 0;
}

int HPPS_ZooIsWorker(int* is_worker) {
  *is_worker = Zoo::Get()->is_worker() ? 1 : 0;
  return 0;
}

int HPPS_ZooIsServer(int* is_server) {
  *is_server = Zoo::Get()->is_server() ? 1 : 0;
  return 0;
}

int HPPS_ZooIsController(int* is_controller) {
  *is_controller = Zoo::Get()->rank() == 0 ? 1 : 0;
  return 0;
}

int HPPS_ZooWorkerId(int* worker_id) {
  *worker_id = Zoo::Get()->rank_to_worker_id(Zoo::Get()->rank());
  return 0;
}

int HPPS_ZooServerId(int* server_id) {
  *server_id = Zoo::Get()->rank_to_server_id(Zoo::Get()->rank());
  return 0;
}

int HPPS_ZooSetLogLevel(int level) {
  LogLevel log_level = static_cast<LogLevel>(level);
  Log::ResetLogLevel(log_level);
  return 0;
}
int HPPS_ZooSetLogFile(const char* file_path) {
  Log::ResetLogFile(file_path);
  return 0;
}
