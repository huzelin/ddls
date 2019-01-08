/*!
 * \file actor.h
 * \brief The actor in HPPS
 */
#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "hpps/common/message.h"
#include "hpps/frame/scheduler/scheduler.h"

namespace std { class thread; }

namespace hpps {

template<typename T> class Queue;

// The basic computation and communication unit in the system
class Actor {
 public:
  explicit Actor(const std::string& name, int thread_num = 1);
  virtual ~Actor();
  // Start to run the Actor
  void Start();
  // Stop to run the Actor
  void Stop();
  // Accept a message from other actors
  void Receive(MessagePtr&);
  // Actor name, a unique identifier of a actor
  const std::string name() const { return name_; }

 protected:
  // Message response function
  using Handler = std::function<void(MessagePtr&)>;
  // Register message handler function
  void RegisterHandler(const MsgType& type, const Handler& task) {
    handlers_.insert({ type, task });
  }
  // Send a message to a dst actor
  void SendTo(const std::string& dst_name, MessagePtr& msg);

  // Main function run in a background thread
  // The default main is to receive msg from other actors and process
  // messages based on registered message handlers
  virtual void Main();

  // message queue
  std::unique_ptr<Queue<MessagePtr> > mailbox_;
  // message handlers function
  std::unordered_map<int, Handler> handlers_;
  bool is_working_;

 private:
  std::string name_;
  int thread_num_;
  std::vector<std::unique_ptr<std::thread>> thread_;

  Actor(const Actor&) = delete;
  void operator=(const Actor&) = delete;

  friend class Scheduler;
};

namespace actor {
const std::string kCommunicator = "communicator";
const std::string kController = "controller";
const std::string kServer = "server";
const std::string kWorker = "worker";
}  // namespace actor

}  // namespace hpps
