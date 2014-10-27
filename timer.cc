#include "timer.h"

namespace sus {
NaiveTimer::NaiveTimer()
    : keep_running_(true) {

}

NaiveTimer::~NaiveTimer() {
  if (scheduler_) {
    keep_running_ = false;
    action_event_.notify_all();
    scheduler_->join();
  }

  CHECK(actions_.empty());
}

void NaiveTimer::Schedule() {
  std::list<Action> expired_actions;

  while (true) {
    {
      boost::unique_lock<boost::mutex> lock(action_guard_);
      while (actions_.empty()) {
        action_event_.timed_wait(lock, boost::posix_time::milliseconds(1000));

        if (!keep_running_)
          return;
      }

      int64 current = sus::GetCurrentTimeInMS();
      Actions::iterator last_unexpired = actions_.upper_bound(current);
      if (last_unexpired != actions_.begin()) {
        for (Actions::iterator expired = actions_.begin();
            expired != last_unexpired; ++expired) {
          expired_actions.push_back(Action());
          expired_actions.back().swap(expired->second);
        }

        actions_.erase(actions_.begin(), last_unexpired);
      }
    }

    if (expired_actions.empty()) {
      SleepInSeconds(1);
      continue;
    }

    for (Action const& actions : expired_actions) {
      for (std::function<void()> const& action : actions)
        action();
    }

    expired_actions.clear();
  }
}

RoughTimer::RoughTimer()
    : keep_running_(true) {

}

RoughTimer::~RoughTimer() {
  if (scheduler_) {
    keep_running_ = false;
    action_event_.notify_all();
    scheduler_->join();
  }

  CHECK(actions_.empty());
}

void RoughTimer::Schedule() {
  std::list<Action> expired_actions;

  while (true) {
    {
      boost::unique_lock<boost::mutex> lock(action_guard_);
      while (actions_.empty()) {
        action_event_.timed_wait(lock, boost::posix_time::milliseconds(1000));

        if (!keep_running_)
          return;
      }

      int64 current = sus::GetCurrentTimeInMS();
      Actions::iterator last_unexpired = actions_.upper_bound(current);
      if (last_unexpired != actions_.begin()) {
        for (Actions::iterator expired = actions_.begin();
            expired != last_unexpired; ++expired) {
          expired_actions.push_back(Action());
          expired_actions.back().swap(expired->second);
        }

        actions_.erase(actions_.begin(), last_unexpired);
      }
    }

    if (expired_actions.empty()) {
      SleepInSeconds(1);
      continue;
    }

    for (Action const& actions : expired_actions) {
      for (std::function<void()> const& action : actions)
        action();
    }

    expired_actions.clear();
  }
}
}
