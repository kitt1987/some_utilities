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

TEST(DiffTest, Normal) {
  std::vector<uint32> a;
  std::set<uint32> b;

  a.push_back(1);
  a.push_back(2);
  a.push_back(3);
  a.push_back(4);
  a.push_back(5);

  std::vector<uint32> a_b, b_a, ab;
  sus::diff(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(a_b),
            std::back_inserter(b_a), std::back_inserter(ab));
  ASSERT_EQ(a_b.size(), a.size());
  ASSERT_TRUE(b_a.empty());
  ASSERT_TRUE(ab.empty());

  ASSERT_TRUE(b.insert(6).second);
  a_b.clear();
  b_a.clear();
  ab.clear();
  sus::diff(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(a_b),
            std::back_inserter(b_a), std::back_inserter(ab));
  ASSERT_EQ(a_b.size(), a.size());
  ASSERT_EQ(b_a.size(), b.size());
  ASSERT_EQ(b_a[0], 6);
  ASSERT_TRUE(ab.empty());

  ASSERT_TRUE(b.insert(2).second);
  ASSERT_TRUE(b.insert(4).second);
  ASSERT_TRUE(b.insert(8).second);

  a_b.clear();
  b_a.clear();
  ab.clear();
  sus::diff(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(a_b),
            std::back_inserter(b_a), std::back_inserter(ab));
  ASSERT_EQ(a_b.size(), 3);
  ASSERT_EQ(a_b[0], 1);
  ASSERT_EQ(a_b[1], 3);
  ASSERT_EQ(a_b[2], 5);
  ASSERT_EQ(b_a.size(), 2);
  ASSERT_EQ(b_a[0], 6);
  ASSERT_EQ(b_a[1], 8);
  ASSERT_EQ(ab.size(), 2);
  ASSERT_EQ(ab[0], 2);
  ASSERT_EQ(ab[1], 4);
}
