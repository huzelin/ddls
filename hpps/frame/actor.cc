/*!
 * \file actor.cc
 * \file The Actor implementation
 */
#include "hpps/frame/actor.h"

#include <chrono>
#include <string>
#include <thread>

#include "hpps/common/message.h"
#include "hpps/common/log.h"
#include "hpps/common/queue.h"
#include "hpps/frame/zoo.h"

namespace hpps {

Actor::Actor(const std::string& name, int thread_num)
    : name_(name), thread_num_(thread_num) {
  mailbox_.reset(new Queue<MessagePtr>());
  Zoo::Get()->RegisterActor(name, this);
  is_working_ = false;
}

Actor::~Actor() {}

void Actor::Start() {
  thread_.resize(thread_num_);
  for (auto i = 0; i < thread_num_; ++i) {
    thread_[i].reset(new std::thread(&Actor::Main, this));
  }
  while (!is_working_) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

void Actor::Stop() {
  while (!mailbox_->Empty()) { ; }
  is_working_ = false;
  mailbox_->Exit();
  for (auto i = 0; i < thread_num_; ++i) {
    thread_[i]->join();
  }
}

void Actor::Receive(MessagePtr& msg) { mailbox_->Push(msg); }

void Actor::Main() {
  is_working_ = true;
  MessagePtr msg;
  while (mailbox_->Pop(msg)) {
    if (handlers_.find(msg->type()) != handlers_.end()) {
      handlers_[msg->type()](msg);
    } else if (handlers_.find(MsgType::Default) != handlers_.end()) {
      handlers_[MsgType::Default](msg);
    } else {
      Log::Fatal("Unexpected msg type: %u\n", msg->type());
    }
  }
}

void Actor::SendTo(const std::string& dst_name, MessagePtr& msg) {
  Zoo::Get()->SendTo(dst_name, msg);
}

}  // namespace hpps
