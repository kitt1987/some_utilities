#pragma once
#include "base.h"

namespace sus {
template<typename K, typename V>
class LRU {
 public:
  LRU(int32 size, int32 clear_factor)
      : size_(size),
        clear_factor_(clear_factor) {
  }

  ~LRU() {
    Clear(size_);
  }

  bool Insert(K const& key, V* value) {
    if (map_.size() >= size_) {
      Clear(clear_factor_);
    }

    if (map_.size() >= size_) {
      return false;
    }

    std::pair<typename DataRelation::iterator, bool> result = map_.insert(
        std::make_pair(key, data_.end()));
    if (!result.second)
      return false;
    result.first->second = data_.insert(data_.end(),
                                        std::make_pair(key, value));
    return true;
  }

  V* Get(K const& key) {
    typename DataRelation::iterator dest = map_.find(key);
    if (dest == map_.end())
      return NULL;

    V* value = dest->second->second;
    data_.push_back(std::make_pair(key, value));
    return value;
  }

 private:
  typedef std::list<std::pair<K, V*> > DataContainer;
  typedef std::map<K, typename DataContainer::iterator> DataRelation;

  int32 size_;
  int32 clear_factor_;
  DataRelation map_;
  DataContainer data_;

  void Clear(int32 factor) {
    if (factor == 0)
      return;

    int32 size_clear = size_ / factor;
    typename DataContainer::iterator last = data_.begin();
    while (size_clear > 0) {
      if (last == data_.end())
        break;

      delete last->second;
      map_.erase(last->first);
      --size_clear;
      last = data_.erase(data_.begin(), last);
    }
  }

  DISALLOW_COPY_AND_ASSIGN(LRU);
};
}
