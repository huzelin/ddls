/*!
 * \file mpi_net.h
 * \brief The mpi net interface
 */
#pragma once

#include "hpps/net/net.h"

#include <limits>
#include <mutex>
#include <queue>
#include <list>

#include "hpps/common/message.h"
#include "hpps/common/dashboard.h"
#include "hpps/common/log.h"
#include "hpps/common/queue.h"

#include <mpi.h>

#ifndef _WIN32
#include <dlfcn.h>
#endif

#ifdef _MSC_VER
#undef max
#endif

namespace hpps {

#define HPPS_MPI_CALL(mpi_return) CHECK((mpi_return) == MPI_SUCCESS)

namespace {

static MPI_Datatype GetDataType(char*)   { return MPI_CHAR; }
static MPI_Datatype GetDataType(int*)    { return MPI_INT; }
static MPI_Datatype GetDataType(float*)  { return MPI_FLOAT; }
static MPI_Datatype GetDataType(double*) { return MPI_DOUBLE; }

static void dlopen_libmpi() {
#ifndef _WIN32
  void *handle = 0;
  int mode = RTLD_NOW | RTLD_GLOBAL;
#if defined(__CYGWIN__)
  /* TODO: Windows */
#elif defined(__APPLE__)
  /* TODO: Mac OS X */
#elif defined(__linux__)
  /* GNU/Linux and others */
#ifdef RTLD_NOLOAD
  mode |= RTLD_NOLOAD;
#endif
  if (!handle) handle = dlopen("libmpi_cxx.so",   mode);
#endif
#endif
}

}  // namespace

class MPINetWrapper : public NetInterface {
 public:
  MPINetWrapper() : /* more_(std::numeric_limits<char>::max()) */ 
      kover_(std::numeric_limits<size_t>::max()) {
  }
  
  struct MPIMsgHandle {
    MPIMsgHandle() : send_buffer(nullptr), send_size(0) { }

    void Wait() {
      MPI_Status status;
      HPPS_MPI_CALL(MPI_Wait(&handle, &status));
    }

    int Test() {
      MPI_Status status;
      int flag;
      HPPS_MPI_CALL(MPI_Test(&handle, &flag, &status));
      return flag;
    }
   
    char* send_buffer;
    long long send_size;
    MPI_Request handle;
  };
  
  void Init(int* argc, char** argv) override {
    // MPI_Init(argc, &argv);
    HPPS_MPI_CALL(MPI_Initialized(&inited_));
    if (!inited_) {
      // NOTICE: Preload libmpi with the right mode. Otherwise python will load it in 
      // a private which will cause errors
      dlopen_libmpi();
      if (argc && *argc == 0) {
        // When using multithread, giving MPI_Init_thread argv with zero length will cause errors.
        HPPS_MPI_CALL(MPI_Init_thread(NULL, NULL, MPI_THREAD_SERIALIZED, &thread_provided_));
      } else {
        HPPS_MPI_CALL(MPI_Init_thread(argc, &argv, MPI_THREAD_SERIALIZED, &thread_provided_));
      }
      HPPS_MPI_CALL(MPI_Initialized(&inited_));
    }
    HPPS_MPI_CALL(MPI_Query_thread(&thread_provided_));
    if (thread_provided_ < MPI_THREAD_SERIALIZED) {
      LOG_FATAL("At least MPI_THREAD_SERIALIZED supported is needed by hpps.");
    }
    else if (thread_provided_ == MPI_THREAD_SERIALIZED) {
      LOG_INFO("hpps MPI-Net is initialized under MPI_THREAD_SERIALIZED mode.");
    }
    else if (thread_provided_ == MPI_THREAD_MULTIPLE) {
      LOG_DEBUG("hpps MPI-Net is initialized under MPI_THREAD_MULTIPLE mode.");
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_);
    MPI_Comm_size(MPI_COMM_WORLD, &size_);
    MPI_Barrier(MPI_COMM_WORLD);
    LOG_DEBUG("%s net util inited, rank = %d, size = %d",
      name().c_str(), rank(), size());
  }

  void Finalize() override { inited_ = 0; MPI_Finalize(); }

  int Bind(int, char*) override { 
    LOG_FATAL("Shouldn't call this in MPI Net"); 
    return -1;
  }

  int Connect(int*, char* [], int) override { 
    LOG_FATAL("Shouldn't call this in MPI Net");
    return -1;
  }
  
  bool active() const { return inited_ != 0; }
  int rank() const override { return rank_; }
  int size() const override { return size_; }
  std::string name() const override { return "MPI"; }

  template <typename ElemType>
  static void Allreduce(ElemType* data, size_t elem_count) {
    MPI_Allreduce(MPI_IN_PLACE, data, (int)elem_count,
      GetDataType(data), MPI_SUM, MPI_COMM_WORLD);
  }

  int Send(MessagePtr& msg) override {
    if (msg.get() == nullptr) return 0;

    MPIMsgHandle* handle = nullptr;
    if (!handles_.empty()) {
      handle = handles_.front();
      if (!handle->Test()) {
        handle = nullptr;
      } else {
        handle->Wait();
        handles_.pop_front();
      }
    }
    if (handle == nullptr) {
      handle = new MPIMsgHandle;
    }
    MessagePtr curr_msg = std::move(msg);
    int size = SerializeAndSend(curr_msg, handle);
    handles_.push_back(handle);
    return size;
  }

  int Recv(MessagePtr* msg) override {
    MPI_Status status;
    int flag;
    // non-blocking probe whether message comes
    HPPS_MPI_CALL(MPI_Iprobe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &flag, &status));
    if (!flag) return 0;
    int count;
    HPPS_MPI_CALL(MPI_Get_count(&status, MPI_BYTE, &count));
    if (count > recv_size_) {
      recv_buffer_ = (char*)realloc(recv_buffer_, count);
      recv_size_ = count;
    }
    // CHECK(count == Message::kHeaderSize);
    return RecvAndDeserialize(status.MPI_SOURCE, count, msg);
  }

  void SendTo(int rank, char* buf, int len) const override {
    if (len <= 0) {
      return;
    }
    MPI_Request send_request;
    MPI_Status status;
    HPPS_MPI_CALL(MPI_Isend(buf, len, MPI_BYTE, rank, 0, 
                          MPI_COMM_WORLD, &send_request));
    HPPS_MPI_CALL(MPI_Wait(&send_request, &status));
  }

  void RecvFrom(int rank, char* buf, int len) const override {
    MPI_Status status;
    int read_cnt = 0;
    while (read_cnt < len) {
      HPPS_MPI_CALL(MPI_Recv(buf + read_cnt, len - read_cnt, MPI_BYTE, 
                           rank, 0, MPI_COMM_WORLD, &status));
      int cur_cnt;
      HPPS_MPI_CALL(MPI_Get_count(&status, MPI_BYTE, &cur_cnt));
      read_cnt += cur_cnt;
    }
  }

  void SendRecv(int send_rank, char* send_data, int send_len,
    int recv_rank, char* recv_data, int recv_len) const {
    MPI_Request send_request;
    // send first, non-blocking
    HPPS_MPI_CALL(MPI_Isend(send_data, send_len, MPI_BYTE, send_rank, 
                          0, MPI_COMM_WORLD, &send_request));
    // then receive, blocking
    MPI_Status status;
    int read_cnt = 0;
    while (read_cnt < recv_len) {
      HPPS_MPI_CALL(MPI_Recv(recv_data + read_cnt, recv_len - read_cnt, MPI_BYTE,
                           recv_rank, 0, MPI_COMM_WORLD, &status));
      int cur_cnt;
      HPPS_MPI_CALL(MPI_Get_count(&status, MPI_BYTE, &cur_cnt));
      read_cnt += cur_cnt;
    }
    // wait for send complete
    HPPS_MPI_CALL(MPI_Wait(&send_request, &status));
  }

  int SerializeAndSend(MessagePtr& msg, MPIMsgHandle* msg_handle) {
    CHECK_NOTNULL(msg_handle);
    MONITOR_BEGIN(MPI_NET_SEND_SERIALIZE);
    int size = sizeof(size_t) + Message::kHeaderSize;
    for (auto& data : msg->data()) 
      size += static_cast<int>(sizeof(size_t) + data.size());
    if (size > msg_handle->send_size) {
      msg_handle->send_buffer = (char*)realloc(msg_handle->send_buffer, size);
      msg_handle->send_size = size;
    }
    memcpy(msg_handle->send_buffer, msg->header(), Message::kHeaderSize);
    char* p = msg_handle->send_buffer + Message::kHeaderSize;
    for (auto& data : msg->data()) {
      size_t s = data.size();
      memcpy(p, &s, sizeof(size_t));
      p += sizeof(size_t);
      memcpy(p, data.data(), s);
      p += s;
    }
    size_t over = kover_; // std::numeric_limits<size_t>::max(); -1;
    memcpy(p, &over, sizeof(size_t));
    MONITOR_END(MPI_NET_SEND_SERIALIZE);

    MPI_Request handle;
    HPPS_MPI_CALL(MPI_Isend(msg_handle->send_buffer, static_cast<int>(size), MPI_BYTE, msg->dst(), 0, MPI_COMM_WORLD, &handle));
    msg_handle->handle = handle;
    return size;
  }

  int RecvAndDeserialize(int src, int count, MessagePtr* msg_ptr) {
    if (!msg_ptr->get()) msg_ptr->reset(new Message());
    MessagePtr& msg = *msg_ptr;
    msg->data().clear();
    MPI_Status status;
    HPPS_MPI_CALL(MPI_Recv(recv_buffer_, count,
      MPI_BYTE, src, 0, MPI_COMM_WORLD, &status));

    MONITOR_BEGIN(MPI_NET_RECV_DESERIALIZE)
    char* p = recv_buffer_;
    size_t s;
    memcpy(msg->header(), p, Message::kHeaderSize);
    p += Message::kHeaderSize;
    memcpy(&s, p, sizeof(size_t));
    p += sizeof(size_t);
    while (s != kover_) {
      Blob data(s);
      memcpy(data.data(), p, data.size());
      msg->Push(data);
      p += data.size();
      memcpy(&s, p, sizeof(size_t));
      p += sizeof(size_t);
    }
    MONITOR_END(MPI_NET_RECV_DESERIALIZE)
    return count;
  }

  int thread_level_support() override { 
    if (thread_provided_ == MPI_THREAD_MULTIPLE) 
      return NetThreadLevel::THREAD_MULTIPLE;
    return NetThreadLevel::THREAD_SERIALIZED; 
  }

 private:
  // const char more_;
  const size_t kover_;
  std::mutex mutex_;
  int thread_provided_;
  int inited_;
  int rank_;
  int size_;
  
  std::list<MPIMsgHandle*> handles_;
  char* recv_buffer_;
  long long recv_size_;
};

}  // namespace hpps
