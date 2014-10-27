#include "thread_pool.h"

namespace sus {
ThreadPool::ThreadPool(char const volatile* static_name, int32 num_threads,
                       int32 num_waiting_task_threshold)
    : num_pending_tasks_(0),
      stop_(false),
      num_threads_(num_threads),
      max_pending_tasks_(num_waiting_task_threshold) {
  ThreadInitial(static_name);
}

ThreadPool::ThreadPool(char const volatile* static_name, int32 num_threads,
                       int32 num_waiting_task_threshold,
                       ContextGenerator const& context_generator,
                       ContextDestructor const& context_destructor)
    : num_pending_tasks_(0),
      stop_(false),
      num_threads_(num_threads),
      max_pending_tasks_(num_waiting_task_threshold),
      ctx_cycle_(context_generator, context_destructor) {
  ThreadInitial(static_name);
}

void ThreadPool::ThreadInitial(char const volatile* static_name) {
  for (int32 i = 0; i < num_threads_; i++) {
    threads_.push_back(
        new boost::thread(
            boost::bind(&ThreadPool::ThreadLoop, this, static_name)));
  }
}

ThreadPool::~ThreadPool() {
}

void ThreadPool::ClearTasks() {
  boost::lock_guard<boost::mutex> lock(task_mutex_);
  while (!task_pool_.empty()) {
    task_pool_.pop();
  }
}

bool ThreadPool::Post(ThreadRunner const& task) {
  boost::lock_guard<boost::mutex> lock(task_mutex_);
  if (num_pending_tasks_ >= max_pending_tasks_) {
    LOG_WARNING << "Too many tasks. Some tasks will be ignored "
                << num_pending_tasks_ << " vs. " << max_pending_tasks_;
    return false;
  }

  task_pool_.push(task);
  num_pending_tasks_++;
  task_signal_.notify_all();
  return true;
}

void ThreadPool::ThreadLoop(char const volatile* static_name) {
  CHECK(static_name != NULL);
  void* context = NULL;
  if (!ctx_cycle_.first.empty())
    context = ctx_cycle_.first();

  while (!stop_) {
    ThreadRunner task;

    {
      boost::unique_lock<boost::mutex> lock(task_mutex_);

      while (num_pending_tasks_ == 0) {
        if (stop_)
          break;

        task_signal_.timed_wait(lock, boost::posix_time::milliseconds(1000));
      }

      if (stop_)
        break;

      if (task_pool_.empty()) {
        LOG_ERROR << "No more tasks, the thread of ThreadPool will exit";
        break;
      }

      task = task_pool_.front();
      task_pool_.pop();
      num_pending_tasks_--;
    }

    task(context);
  }

  if (context != NULL && !ctx_cycle_.second.empty())
    ctx_cycle_.second(context);
}
}
