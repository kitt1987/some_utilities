#include <gtest/gtest.h>
#include "red_black_tree.h"
#include "util.h"

TEST(TreeTest, RedBlackTreeInsertionAndDeletion) {
  uint32 tree_size = 50000;

  std::vector<uint32> keys(tree_size);
  uint32 seed = 10;
  for (uint32& elem : keys) {
    elem = seed;
    ++seed;
  }

  sus::NaiveShuffle(&keys);
  ASSERT_FALSE(std::is_sorted(keys.begin(), keys.end()));

  sus::RedBlackTree<uint32, uint32> tree;
  for (auto key : keys) {
    ASSERT_TRUE(tree.Insert(key, 1))<< key;
    if (!tree.Verify()) {
      tree.Output();
      ASSERT_EQ(key, -1);
    }
  }

  ASSERT_EQ(static_cast<int32>(tree_size), tree.Size());
  std::vector<uint32> sorted_keys = tree.InorderInterate();
  ASSERT_EQ(sorted_keys.size(), keys.size());
  ASSERT_TRUE(std::is_sorted(sorted_keys.begin(), sorted_keys.end()));

  sus::NaiveShuffle(&keys);
  ASSERT_EQ(sorted_keys.size(), keys.size());

  for (auto key : keys) {
    ASSERT_EQ(1, tree.Delete(key))<< key;
    if (!tree.Verify()) {
      tree.Output();
      ASSERT_EQ(key, -1);
    }
  }

  ASSERT_EQ(tree.Size(), 0);
}
