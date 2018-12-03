/*!
 * \file timer.cc
 * \brief The timer implementation
 */
#include "hpps/common/timer.h"

namespace hpps {

Timer::Timer() {
  Start();
}

void Timer::Start() {
  start_point_ = Clock::now();
}

double Timer::Elapse() {
  TimePoint end_point = Clock::now();
  std::chrono::duration<double, std::milli> time_ms =
      end_point - start_point_;
  return time_ms.count();
}

}  // namespace hpps
