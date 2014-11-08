#include <gtest/gtest.h>
#include <random>
#include <iterator>
#include <iostream>
#include "util.h"
#include "wall_timer.h"

TEST(ShuffleTest, SimpleShuffle) {
  std::vector<uint32> seq(20);
  uint32 seed = 10;
  for (uint32& elem : seq) {
    elem = seed;
    ++seed;
  }

  ASSERT_TRUE(std::is_sorted(seq.begin(), seq.end()));
  sus::NaiveShuffle(&seq);
  ASSERT_FALSE(std::is_sorted(seq.begin(), seq.end()));
}

TEST(ShuffleTest, BenchStdShuffle) {
  std::vector<uint32> seq(2000000);
  uint32 seed = 10;
  for (uint32& elem : seq) {
    elem = seed;
    ++seed;
  }

  std::vector<uint32> seq_for_std = seq;

  sus::WallTimer timer;
  sus::NaiveShuffle(&seq);
  uint64 elapse = timer.ElapseInMs();

//  std::copy(seq.begin(), seq.end(), std::ostream_iterator<int>(std::cout, " "));
//  std::cout << std::endl;

  LOG_WARNING << "NaiveShuffle takes " << elapse << "ms";

  std::random_device rd;
  std::mt19937 g(rd());

  timer.Restart();
  std::shuffle(seq_for_std.begin(), seq_for_std.end(), g);
  elapse = timer.ElapseInMs();

//  std::copy(seq_for_std.begin(), seq_for_std.end(),
//            std::ostream_iterator<int>(std::cout, " "));
//  std::cout << std::endl;

  LOG_WARNING << "std shuffle takes " << elapse << "ms";
}
