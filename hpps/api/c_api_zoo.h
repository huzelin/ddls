/*
 * \file c_api_zoo.h
 */
#ifndef H_CAPI_HPPS_H_
#define H_CAPI_HPPS_H_

#ifdef __cplusplus
extern "C" {
#endif

int HPPS_ZooStart(const char* args);
int HPPS_ZooStop();
int HPPS_ZooBarrier();

int HPPS_ZooRank(int* rank);
int HPPS_ZooSize(int* size);

int HPPS_ZooNumWorkers(int* num_workers);
int HPPS_ZooNumServers(int* num_servers);

int HPPS_ZooIsWorker(int* is_worker);
int HPPS_ZooIsServer(int* is_server);
int HPPS_ZooIsController(int* is_controller);

int HPPS_ZooWorkerId(int* worker_id);
int HPPS_ZooServerId(int* server_id);

int HPPS_ZooSetLogLevel(int level);
int HPPS_ZooSetLogFile(const char* file_path);

#ifdef __cplusplus
}
#endif

#endif  // H_CAPI_HPPS_H_
