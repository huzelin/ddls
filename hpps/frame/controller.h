/*!
 * \file controller.h
 * \brief The controller implementation
 */
#pragma once

#include "hpps/common/message.h"
#include "hpps/frame/actor.h"

namespace hpps {

class Controller : public Actor {
 public:
  Controller();
  ~Controller();

 private:
  void ProcessBarrier(MessagePtr& msg);
  void ProcessRegister(MessagePtr& msg);

  class RegisterController;
  RegisterController* register_controller_;
  class BarrierController;
  BarrierController* barrier_controller_;
};

}  // namespace hpps
