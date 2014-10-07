#pragma once
#include "base.h"

#ifdef HAS_BOOST

#include <boost/thread.hpp>

namespace sus {
template<typename ContextType>
void* ContextGeneratorTemplate() {
  return new ContextType;
}

template<typename ContextType>
void ContextDestructorTemplate(void* ctx) {
  delete reinterpret_cast<ContextType*>(ctx);
}

class ThreadPool {
 public:
  typedef boost::function<void(void*)> ThreadRunner;
  typedef boost::function<void*()> ContextGenerator;
  typedef boost::function<void(void*)> ContextDestructor;

  ThreadPool(char const volatile* static_name, int32 num_threads,
             int32 num_waiting_task_threshold);
  ThreadPool(char const volatile* static_name, int32 num_threads,
             int32 num_waiting_task_threshold,
             ContextGenerator const& context_generator,
             ContextDestructor const& context_destructor);
  virtual ~ThreadPool();
  virtual bool Post(ThreadRunner const& task);
  void Stop() {
    stop_ = true;
    task_signal_.notify_all();
    for (ThreadIterator each = threads_.begin(); each != threads_.end();
        each++) {
      (*each)->join();
      delete (*each);
    }

    threads_.clear();

    ClearTasks();
  }

 private:
  typedef std::queue<ThreadRunner> TaskQueue;
  TaskQueue task_pool_;
  int32 num_pending_tasks_;
  boost::mutex task_mutex_;
  boost::condition_variable task_signal_;
  bool stop_;
  int32 num_threads_;
  int32 max_pending_tasks_;
  std::pair<ContextGenerator, ContextDestructor> ctx_cycle_;

  typedef std::vector<boost::thread*> ThreadSet;
  typedef ThreadSet::iterator ThreadIterator;
  ThreadSet threads_;

  void ThreadLoop(char const volatile* static_name);
  void ClearTasks();
  void ThreadInitial(char const volatile* static_name);

  DISALLOW_COPY_AND_ASSIGN(ThreadPool);
};
}
#endif
