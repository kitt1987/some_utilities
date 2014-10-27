#include "timer.h"
#include "wall_timer.h"
#include "util.h"
#include <gflags/gflags.h>
#include <stdlib.h>
#include <iostream>
#include <gperftools/profiler.h>

DEFINE_int32(round, 100000, "");
DEFINE_int32(inline_times, 0, "");
DEFINE_bool(random_delay, false, "");
DEFINE_int32(max_delay, 10, "");
DEFINE_bool(profile, false, "Enable profiler");

std::vector<int64> deltas;

void Callback(sus::NaiveTimer* timer, int64 expected, uint32 delay,
              int32 inline_times) {
  int64 current = sus::GetCurrentTimeInMS();
  int64 delta = current - expected;
  CHECK_GE(delta, 0);
  deltas.push_back(delta);
  if (inline_times > 0) {
    timer->Add(
        delay,
        std::bind(Callback, timer, current + delay, delay, --inline_times));
  }
}

void TestNaiveTimer() {
  std::vector<uint32> delays;
  if (FLAGS_random_delay) {
    delays = sus::NaiveRandomNumbers(FLAGS_max_delay, FLAGS_round);
  }

  int64 current = sus::GetCurrentTimeInMS();
  sus::WallTimer wall_timer;
  {
    if (FLAGS_profile)
      ProfilerStart("NaiveTimer");

    sus::NaiveTimer timer;
    timer.Run();
    wall_timer.Start();

    if (delays.empty()) {
      for (int i = 0; i < FLAGS_round; ++i) {
        timer.Add(
            FLAGS_max_delay,
            std::bind(Callback, &timer, current + FLAGS_max_delay * 1000,
                      FLAGS_max_delay, FLAGS_inline_times));
      }
    } else {
      for (int i = 0; i < FLAGS_round; ++i) {
        timer.Add(
            delays[i],
            std::bind(Callback, &timer, current + delays[i] * 1000, delays[i],
                      FLAGS_inline_times));
      }
    }
  }

  if (FLAGS_profile) {
    ProfilerStop();
    ProfilerFlush();
  }

  uint64 elapse = wall_timer.ElapseInMs();
  LOG_WARNING<< FLAGS_round << " rounds take " << elapse << "ms";
  int32 delta_less_than_1s = 0, delta_less_than_5s = 0, delta_less_than_10s = 0,
      delta_greater_than_10s = 0;
  for (auto delta : deltas) {
    if (delta < 1000) {
      ++delta_less_than_1s;
    } else if (delta < 5000) {
      ++delta_less_than_5s;
    } else if (delta < 10000) {
      ++delta_less_than_10s;
    } else {
      ++delta_greater_than_10s;
    }
  }

  LOG_WARNING<< "Delta distribution:" << delta_less_than_1s << "(<1),"
  << delta_less_than_5s << "(<5)," << delta_less_than_10s
  << "(<10)," << delta_greater_than_10s << "(>10)";
}

int main(int argc, char** argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, true);
  TestNaiveTimer();
  return 0;
}
