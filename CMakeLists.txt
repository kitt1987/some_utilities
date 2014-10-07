cmake_minimum_required (VERSION 2.6)
project (libsus)

file(GLOB TOP_SRCS "*.cc")
set(LIB_SOURCES ${TOP_SRCS})

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
#set(CLASSIC_LIB_DIR /usr/local/lib /usr/lib /usr/lib/x86_64-linux-gnu /lib/x86_64-linux-gnu)

find_package (Boost)
if (BOOST_FOUND)
	add_definitions(-DHAS_BOOST)
endif (BOOST_FOUND)

find_path(GLOG_INCLUDE_DIR glog/logging.h HINTS ${CLASSIC_INCLUDE_DIR})

if (GLOG_INCLUDE_DIR-NOTFOUND)
	message(STATUS "Glog not found")
else (GLOG_INCLUDE_DIR-NOTFOUND)
	add_definitions(-DHAS_GOOGLE_GLOG)
endif (GLOG_INCLUDE_DIR-NOTFOUND)

set(LIBRARY_OUTPUT_PATH "out")

include_directories("base" ${GLOG_INCLUDE_DIR})
add_library(${LIB_NAME} STATIC ${LIB_SOURCES})
