#include "dir.h"
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

namespace sus {
Dir::Dir(std::string const& dir)
    : dp_(NULL) {
  CHECK(TryToOpen(dir)) << dir;
}

Dir::~Dir() {
  if (dp_ != NULL)
    closedir(reinterpret_cast<DIR*>(dp_));
}

bool Dir::TryToOpen(std::string const& dir) {
  dp_ = opendir(dir.c_str());
  if (dp_ == NULL) {
    LOG_ERROR << "Fail to open " << dir << " due to [" << errno << "]"
              << strerror(errno);
    return false;
  }

  return true;
}

void Dir::GetAll(std::vector<std::string>* subs) {
  CHECK(dp_ != NULL);
  struct dirent entry;
  struct dirent* result = NULL;
  DIR* dp = reinterpret_cast<DIR*>(dp_);
  while (readdir_r(dp, &entry, &result) == 0) {
    if (result == NULL)
      break;

    if (entry.d_name[0] == '.'
        && (entry.d_name[1] == '\0'
            || (entry.d_name[1] == '.' && entry.d_name[2] == '\0')))
      continue;

    subs->push_back(entry.d_name);
  }
}
}
