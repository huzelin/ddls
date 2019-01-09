/*!
 * \file table_interface.h
 * \brief The table interface
 */
#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <cctype>

#include "hpps/common/blob.h"
#include "hpps/common/message.h"
#include "hpps/common/random.h"

namespace std { class mutex; }

namespace hpps {

typedef int32_t integer_t;

class Waiter;
struct AddOption;
struct GetOption;
enum MsgType;

// User implementent this
class WorkerTable {
 public:
  WorkerTable();
  virtual ~WorkerTable();

  void Get(Blob keys, const GetOption* option = nullptr);
  void Add(Blob keys, Blob values, const AddOption* option = nullptr);

  int GetAsync(Blob keys, const GetOption* option = nullptr);
  int AddAsync(Blob keys, Blob values, const AddOption* option = nullptr);

  void Wait(int id);

  void Reset(int msg_id, int num_wait);

  void Notify(int id);

  virtual int Partition(const std::vector<Blob>& kv,
   MsgType partition_type,
   std::unordered_map<int, std::vector<Blob> >* out) = 0;

  virtual void ProcessReplyGet(std::vector<Blob>&) = 0;

  // add user defined data structure
 private:
  std::string table_name_;
  // assuming there are at most 2^32 tables
  int table_id_;
  std::mutex* m_;
  std::vector<Waiter*> waitings_;
  // assuming there are at most 2^32 msgs waiting in line
  int msg_id_;
};

class Stream;

// interface for checkpoint table
class Serializable {
 public:
  virtual void Store(Stream* s) = 0;
  virtual void Load(Stream* s) = 0;
};

// describe the server parameter storage data structure and related method
class ServerTable : public Serializable {
 public:
  ServerTable(const std::string& ps_mode);
  virtual ~ServerTable() = default;
  virtual void ProcessAdd(const std::vector<Blob>& data) = 0;
  virtual void ProcessGet(const std::vector<Blob>& data,
                          std::vector<Blob>* result) = 0;

  const std::string& ps_mode() const { return ps_mode_; }

 protected:
  std::string ps_mode_;
};

template <typename DType>
class ParamInitializer {
 public:
  void ResetRandomOption(const RandomOption& random_option) {
    random_.SetRandomOption(random_option);
  }

 protected:
  Random<DType> random_;
};

#define DEFINE_TABLE_TYPE(template_type, worker_table_type,  server_table_type) \
  typedef worker_table_type<template_type> WorkerTableType;                     \
  typedef server_table_type<template_type> ServerTableType;

}  // namespace hpps
