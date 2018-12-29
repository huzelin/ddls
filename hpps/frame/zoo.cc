/*!
 * \file zoo.cc
 * \brief The zoo implementation
 */
#include "hpps/frame/zoo.h"

#include <string>

#include "hpps/frame/actor.h"
#include "hpps/frame/communicator.h"
#include "hpps/frame/controller.h"
#include "hpps/common/dashboard.h"
#include "hpps/common/message.h"
#include "hpps/net/net.h"
#include "hpps/frame/server.h"
#include "hpps/common/configure.h"
#include "hpps/common/log.h"
#include "hpps/common/queue.h"
#include "hpps/frame/worker.h"

namespace hpps {

Zoo::Zoo() { }

Zoo::~Zoo() { }

HPPS_DEFINE_string(ps_role, "default", "none / worker / server / default");
HPPS_DEFINE_bool(ma, false, "model average, will not start server if true");
HPPS_DECLARE_bool(sync);

namespace {

int ParsePSRole(const std::string& ps_role) {
  if (ps_role == "none")    return Role::NONE;
  if (ps_role == "worker")  return Role::WORKER; 
  if (ps_role == "server")  return Role::SERVER;
  if (ps_role == "default") return Role::ALL;
  return -1;
}

const int kController = 0;

}  // namespace

Zoo* Zoo::Get() {
  static Zoo zoo;
  return &zoo;
}

void Zoo::Start(int* argc, char** argv) {
  LOG_DEBUG("Zoo started\n");
  ParseCMDFlags(argc, argv);

  // Init the network
  net_util_ = NetInterface::Get();
  net_util_->Init(argc, argv);

  if (!HPPS_CONFIG_ma) { StartPS(); }
}

void Zoo::Stop(bool finalize_net) {
  // Stop the system
  if (!HPPS_CONFIG_ma) { StopPS(); }
  // Stop the network
  if (finalize_net) net_util_->Finalize();
  for (auto actor : zoo_) delete actor.second;
  zoo_.clear();
  LOG_INFO("Parameter Server Shutdown successfully");
}

int Zoo::rank() const { return NetInterface::Get()->rank(); }
int Zoo::size() const { return NetInterface::Get()->size(); }

void Zoo::SendTo(const std::string& name, MessagePtr& msg) {
  CHECK(zoo_.find(name) != zoo_.end());
  zoo_[name]->Receive(msg);
}
void Zoo::Receive(MessagePtr& msg) {
  mailbox_->Push(msg);
}

void Zoo::StartPS() {
  int role = ParsePSRole(HPPS_CONFIG_ps_role);
  CHECK(role != -1);

  nodes_.resize(size());
  nodes_[rank()].rank = rank();
  nodes_[rank()].role = role;
  mailbox_.reset(new Queue<MessagePtr>);

  // NOTE(feiga): the start order is non-trivial, communicator should be last.
  if (rank() == kController) { 
    Actor* controler = new Controller(); 
    controler->Start(); 
  }
  Actor* communicator = new Communicator();
  communicator->Start();
  // activate the system
  RegisterNode();

  if (node::is_server(role)) {
    Actor* server = Server::GetServer();
    server->Start();
  }
  if (node::is_worker(role)) {
    Actor* worker = new Worker();
    worker->Start();
  }
  Barrier();
  LOG_INFO("Rank %d: Parameter Server start successfully", rank());
}

void Zoo::StopPS() {
  if (HPPS_CONFIG_sync) {
    FinishTrain();
  }
  Barrier();

  // Stop all actors
  for (auto actor : zoo_) { 
    actor.second->Stop(); 
  }
}

void Zoo::RegisterNode() {
  MessagePtr msg(new Message());
  msg->set_src(rank());
  msg->set_dst(kController);
  msg->set_type(MsgType::Control_Register);
  msg->Push(Blob(&nodes_[rank()], sizeof(Node)));
  SendTo(actor::kCommunicator, msg);

  // waif for reply
  mailbox_->Pop(msg);
  CHECK(msg->type() == MsgType::Control_Reply_Register);
  CHECK(msg->data().size() == 2);
  Blob info_blob = msg->data()[0];
  Blob count_blob = msg->data()[1];
  num_workers_ = count_blob.As<int>(0);
  num_servers_ = count_blob.As<int>(1);
  worker_id_to_rank_.resize(num_workers_);
  server_id_to_rank_.resize(num_servers_);
  CHECK(info_blob.size() == size() * sizeof(Node));
  memcpy(nodes_.data(), info_blob.data(), info_blob.size());
  for (auto node : nodes_) {
    if (node.worker_id != -1) {
      worker_id_to_rank_[node.worker_id] = node.rank;
    }
    if (node.server_id != -1) {
      server_id_to_rank_[node.server_id] = node.rank;
    }
  }
  LOG_DEBUG("rank %d end register\n", Zoo::Get()->rank());
}

void Zoo::RegisterActor(const std::string name, Actor* actor) {
  CHECK(zoo_[name] == nullptr);
  zoo_[name] = actor;
}

void Zoo::FinishTrain() {
  for (auto i = 0; i < num_servers_; i++) {
    int dst_rank = server_id_to_rank(i);
    MessagePtr msg(new Message());
    msg->set_src(rank());
    msg->set_dst(dst_rank);
    msg->set_type(MsgType::Server_Finish_Train);
    SendTo(actor::kCommunicator, msg);
  }
}


void Zoo::Barrier() {
  MessagePtr msg(new Message());
  msg->set_src(rank());
  msg->set_dst(kController); 
  msg->set_type(MsgType::Control_Barrier);
  SendTo(actor::kCommunicator, msg);

  LOG_DEBUG("rank %d requested barrier.\n", rank());
  // wait for reply
  mailbox_->Pop(msg);
  CHECK(msg->type() == MsgType::Control_Reply_Barrier);
  LOG_DEBUG("rank %d reached barrier\n", rank());
}

int Zoo::RegisterTable(WorkerTable* worker_table) {
  return dynamic_cast<Worker*>(zoo_[actor::kWorker])
    ->RegisterTable(worker_table);
}

int Zoo::RegisterTable(ServerTable* server_table) {
  return dynamic_cast<Server*>(zoo_[actor::kServer])
    ->RegisterTable(server_table);
}

}  // namespace hpps
