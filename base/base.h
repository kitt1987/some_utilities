#pragma once

#include "discopiable.h"
#include "data_types.h"
#include <algorithm>
#include <iterator>
#include <memory>
#include <set>
#include <functional>
#include <map>
#include <list>
#include <iostream>

#ifndef HAS_GOOGLE_GLOG

#include <cassert>
#define LOG_FATAL \
  std::cerr << ""

#define LOG_ERROR \
  std::cerr << ""

#define LOG_WARNING \
  std::cerr << ""

#define CHECK(condition) \
  assert((condition));\
  if (!(condition)) \
    std::cerr << ""

#define CHECK_EQ(val1, val2) \
  assert(val1 == val2); \
  if (val1 != val2) \
    std::cerr << ""

#define CHECK_GT(val1, val2) \
  assert(val1 > val2); \
  if (!(val1 > val2)) \
    std::cerr << ""

#define CHECK_NE(val1, val2) \
  assert(val1 != val2); \
  if (val1 == val2) \
    std::cerr << ""
#else
#include <glog/logging.h>
#define LOG_FATAL (LOG(FATAL))
#define LOG_ERROR (LOG(ERROR))
#define LOG_WARNING (LOG(WARNING))
#endif
