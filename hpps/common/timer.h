/*!
 * \file timer.h
 * \brief The timer in hpps
 */
#pragma once

#include <chrono>
#include <string>

namespace hpps {

class Timer {
 public:
  Timer();

  // Restart the timer
  void Start();

  // Get elapsed milliseconds since last Timer::Start
  double Elapse();

 private:
  using Clock = std::chrono::high_resolution_clock;
  using TimePoint = Clock::time_point;

  TimePoint start_point_;
};

}  // namespace hpps
