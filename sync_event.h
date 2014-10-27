#pragma once
#include "base.h"
#include <boost/thread.hpp>

namespace sus {
class SyncEvent {
 public:
  SyncEvent()
      : value_(false) {
  }

  ~SyncEvent() {
  }

  void Wait() {
    boost::unique_lock<boost::mutex> lock(mutex_);
    while (!value_) {
      signal_.wait(lock);
    }

    CHECK(value_) << "Damn condition variable";
  }

  bool TimedWait(uint32 duration_in_ms) {
    boost::unique_lock<boost::mutex> lock(mutex_);
    if (!value_) {
      return signal_.timed_wait(lock,
                                boost::posix_time::milliseconds(duration_in_ms));
    }

    return true;
  }

  void Notify() {
    {
      boost::lock_guard<boost::mutex> lock(mutex_);
      value_ = true;
    }

    signal_.notify_all();
  }

 private:
  boost::mutex mutex_;
  boost::condition_variable signal_;
  bool value_;

  DISALLOW_COPY_AND_ASSIGN(SyncEvent);
};
}
