#include <gtest/gtest.h>
#include <sstream>
#include <iostream>
#include "red_black_tree.h"
#include "util.h"

TEST(TreeTest, RedBlackTreeInsertionAndDeletion) {
  uint32 tree_size = 1000;
  uint32 max_key = 10000;
  sus::RedBlackTree<uint32, uint32> tree;
  std::vector<uint32> keys = sus::NaiveRandomNumbers(max_key, tree_size);
  for (auto key : keys)
    tree.Insert(key, 0);

  std::vector<uint32> sorted_keys = tree.InorderInterate();
//  std::stringstream sorted;
//  for (auto sorted_key : sorted_keys) {
//    sorted << sorted_key << ",";
//  }
//
//  std::cerr << sorted.str() << std::endl;
  ASSERT_TRUE(std::is_sorted(sorted_keys.begin(), sorted_keys.end()));
}
