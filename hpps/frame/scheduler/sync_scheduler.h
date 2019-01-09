/*!
* \file sync_schedule.h
* \brief The sync schedule
*/
#pragma once

#include "hpps/frame/scheduler/scheduler.h"

#include <algorithm>
#include <string>
#include <vector>

#include "hpps/common/queue.h"
#include "hpps/frame/zoo.h"

namespace hpps {

class SyncScheduler : public Scheduler {
 public:
  explicit SyncScheduler(ServerTable* server_table, Actor* server);

  virtual void ProcessGet(MessagePtr& msg);
  virtual void ProcessAdd(MessagePtr& msg);
  virtual void ProcessFinishTrain(MessagePtr& msg);

  // make some modification to suit to the sync server
  // please not use in other place, may different with the general vector clock
  class VectorClock {
   public:
    explicit VectorClock(int n) : 
      local_clock_(n, 0), global_clock_(0), size_(0) {}

    // Return true when all clock reach a same number
    virtual bool Update(int i) {
      ++local_clock_[i];
      if (global_clock_ < *(std::min_element(std::begin(local_clock_),
        std::end(local_clock_)))) {
        ++global_clock_;
        if (global_clock_ == max_element()) {
          return true;
        }
      }
      return false;
    }

    virtual bool FinishTrain(int i) {
      local_clock_[i] = std::numeric_limits<int>::max();
      if (global_clock_ < min_element()) {
        global_clock_ = min_element();
        if (global_clock_ == max_element()) {
          return true;
        }
      }
      return false;
    }

    std::string DebugString() {
      std::string rank = "rank :" + std::to_string(Zoo::Get()->rank());
      std::string os = rank + " global ";
      os += std::to_string(global_clock_) + " local: ";
      for (auto i : local_clock_) { 
        if (i == std::numeric_limits<int>::max()) os += "-1 ";
        else os += std::to_string(i) + " ";
      }
      return os;
    }

    int local_clock(int i) const { return local_clock_[i]; }
    int global_clock() const { return global_clock_; }
   
   private:
    int max_element() const {
      int max = global_clock_;
      for (auto val : local_clock_) {
        max = (val != std::numeric_limits<int>::max() && val > max) ? val : max;
      }
      return max;
    }
    int min_element() const {
      return *std::min_element(std::begin(local_clock_), std::end(local_clock_));
    }
   
   protected:
    std::vector<int> local_clock_;
    int global_clock_;
    int size_;
  };
 
 protected:
  std::unique_ptr<VectorClock> worker_get_clocks_;
  std::unique_ptr<VectorClock> worker_add_clocks_;
  std::vector<int> num_waited_add_;

  Queue<MessagePtr> msg_add_cache_;
  Queue<MessagePtr> msg_get_cache_;
};

}  // namespace hpps

