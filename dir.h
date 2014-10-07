#pragma once
#include "base.h"

namespace sus {
class Dir {
 public:
  explicit Dir(std::string const& dir);
  ~Dir();

  bool TryToOpen(std::string const& dir);
  void GetAll(std::vector<std::string>* subs);

 private:
  void* dp_;

  DISALLOW_COPY_AND_ASSIGN(Dir);
};
}
