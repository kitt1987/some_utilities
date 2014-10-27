#pragma once

#include "base.h"
#include "util.h"
#include "red_black_tree.h"
#include <boost/thread.hpp>

namespace sus {
class NaiveTimer {
 public:
  NaiveTimer();
  ~NaiveTimer();

  template<typename ActionType>
  void Add(uint32 delay, ActionType const& action) {
    boost::lock_guard<boost::mutex> lock(action_guard_);
    actions_[sus::GetCurrentTimeInMS() + static_cast<int64>(delay * 1000)]
        .push_back(action);
    if (actions_.size() == 1)
      action_event_.notify_all();
  }

  template<typename ActionType>
  void Add(uint32 delay, ActionType&& action) {
    boost::lock_guard<boost::mutex> lock(action_guard_);
    actions_[sus::GetCurrentTimeInMS() + static_cast<int64>(delay * 1000)]
        .push_back(std::move(action));
    if (actions_.size() == 1)
      action_event_.notify_all();
  }

  void Run() {
    scheduler_.reset(
        new boost::thread(boost::bind(&NaiveTimer::Schedule, this)));
  }

  void Clear() {
    boost::lock_guard<boost::mutex> lock(action_guard_);
    actions_.clear();
  }

 private:
  typedef std::vector<std::function<void()>> Action;
  typedef std::map<int64, Action> Actions;
  Actions actions_;
  mutable boost::mutex action_guard_;
  boost::condition_variable action_event_;
  bool keep_running_;
  std::unique_ptr<boost::thread> scheduler_;

  void Schedule();

  DISALLOW_COPY_AND_ASSIGN(NaiveTimer);
};

class RoughTimer {
 public:
  RoughTimer();
  ~RoughTimer();

  template<typename ActionType>
  void Add(uint32 delay, ActionType const& action) {
    boost::lock_guard<boost::mutex> lock(action_guard_);
    actions_[sus::GetCurrentTimeInMS() + static_cast<int64>(delay * 1000)]
        .push_back(action);
    if (actions_.size() == 1)
      action_event_.notify_all();
  }

  template<typename ActionType>
  void Add(uint32 delay, ActionType&& action) {
    boost::lock_guard<boost::mutex> lock(action_guard_);
    actions_[sus::GetCurrentTimeInMS() + static_cast<int64>(delay * 1000)]
        .push_back(std::move(action));
    if (actions_.size() == 1)
      action_event_.notify_all();
  }

  void Run() {
    scheduler_.reset(
        new boost::thread(boost::bind(&RoughTimer::Schedule, this)));
  }

  void Clear() {
    boost::lock_guard<boost::mutex> lock(action_guard_);
    actions_.clear();
  }

 private:
  typedef std::vector<std::function<void()>> Action;
  typedef std::map<int64, Action> Actions;
  Actions actions_;
  mutable boost::mutex action_guard_;
  boost::condition_variable action_event_;
  bool keep_running_;
  std::unique_ptr<boost::thread> scheduler_;

  void Schedule();

  DISALLOW_COPY_AND_ASSIGN(RoughTimer);
};
}
