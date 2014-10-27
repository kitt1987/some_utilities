cmake_minimum_required (VERSION 2.6)
project (libsus)

set(LIB_SOURCES
  dir.cc
  random_file.cc
  tiny_read_only_file.cc
  write_only_file.cc
)

set(LIB_NAME sus)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
  set(LIB_NAME ${LIB_NAME}_debug)
endif()

set(CXX_FLAGS
  -g
  -Wall
  -Wextra
  -Wconversion
  -Wno-unused-parameter
  -Wold-style-cast
  -Woverloaded-virtual
  -Wpointer-arith
  -Wshadow
  -Wwrite-strings
  -std=c++0x
  -rdynamic
)

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG")

set(CLASSIC_INCLUDE_DIR /usr/local/include /usr/include)

find_package (Boost)
if (Boost_FOUND)
	set(LIB_SOURCES ${LIB_SOURCES}
	  thread_pool.cc
	  timer.cc
	)
endif (Boost_FOUND)

find_path(GLOG_INCLUDE_DIR glog/logging.h HINTS ${CLASSIC_INCLUDE_DIR})

if (GLOG_INCLUDE_DIR-NOTFOUND)
	message(STATUS "Glog not found")
else (GLOG_INCLUDE_DIR-NOTFOUND)
	add_definitions(-DHAS_GOOGLE_GLOG)
endif (GLOG_INCLUDE_DIR-NOTFOUND)

set(LIBRARY_OUTPUT_PATH "out")

include_directories("base" ${GLOG_INCLUDE_DIR})
add_library(${LIB_NAME} STATIC ${LIB_SOURCES})

include_directories(.)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/out /usr/local/lib)

if(${CMAKE_BUILD_TYPE} MATCHES "Release")
add_executable(timer_bench "benchmark/bench_timer.cc")
target_link_libraries(timer_bench libsus.a boost_thread-mt boost_system-mt gflags glog tcmalloc profiler)
else(${CMAKE_BUILD_TYPE} MATCHES "Release")
add_executable(unit.test "test/tree_test.cc")
target_link_libraries(unit.test libsus_debug.a gtest gtest_main boost_thread-mt boost_system-mt gflags glog)
endif(${CMAKE_BUILD_TYPE} MATCHES "Release")
