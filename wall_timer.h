#pragma once

#include "base.h"

#ifdef WIN32
#include <windows.h>
#else
#include <strings.h>
#include <sys/time.h>
#include <errno.h>
#include <unistd.h>
#endif

namespace sus {
class WallTimer {
 public:
  WallTimer() {
    Start();
  }

  ~WallTimer() {
  }

  void Start() {
    last_point_ = CurrentTimeInUs();
  }

  void Restart() {
    Start();
  }

  uint64 ElapseInMs() {
    uint64 current = CurrentTimeInUs();
    CHECK_GE(current, last_point_);
    return (current - last_point_) / 1000;
  }

 private:
  uint64 last_point_;

  static uint64 CurrentTimeInMs() {
#ifdef WIN32
    SYSTEMTIME c;
    GetSystemTime(&c);

    FILETIME c_number;
    CHECK(SystemTimeToFileTime(&c, &c_number));
    uint64 timestamp = c_number.dwHighDateTime;
    timestamp <<= 32;
    timestamp += c_number.dwLowDateTime;
    return timestamp;
#else
    struct timeval value;
    bzero(&value, sizeof value);
    CHECK_EQ(0, gettimeofday(&value, NULL))<< errno << ":" << strerror(errno);
    return value.tv_sec * 1000 + value.tv_usec / 1000;
#endif
  }

  static uint64 CurrentTimeInUs() {
    struct timeval value;
    bzero(&value, sizeof value);
    CHECK_EQ(0, gettimeofday(&value, NULL))<< errno << ":" << strerror(errno);
    return value.tv_sec * 1000000 + value.tv_usec;
  }

  DISALLOW_COPY_AND_ASSIGN(WallTimer);
};
}
