#include "file.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

namespace sus {
TinyReadOnlyFile::TinyReadOnlyFile()
    : fd_(-1),
      data_(NULL),
      size_(0) {
}

TinyReadOnlyFile::TinyReadOnlyFile(std::string const& name)
    : fd_(-1),
      data_(NULL),
      size_(0) {
  TryToOpen(name);
}

TinyReadOnlyFile::TinyReadOnlyFile(char const* name)
    : fd_(-1),
      data_(NULL),
      size_(0) {
  TryToOpen(name);
}

TinyReadOnlyFile::~TinyReadOnlyFile() {
  if (data_ == NULL)
    return;

  CHECK_GT(fd_, -1);
  if (munmap(data_, static_cast<unsigned long>(size_)) < 0) {
    LOG_ERROR << "Fail to unmap file[" << fd_ << "] due to " << "[" << errno
              << "]" << strerror(errno);
  }

  if (close(fd_) < 0) {
    LOG_ERROR << "Fail to close file[" << fd_ << "] due to " << "[" << errno
              << "]" << strerror(errno);
  }
}

bool TinyReadOnlyFile::TryToOpen(char const* name) {
  struct stat state;
  bzero(&state, sizeof state);

  if (stat(name, &state) < 0) {
    LOG_ERROR << "Fail to stat " << name << " due to " << "[" << errno << "]"
              << strerror(errno);
    return false;
  }

  fd_ = open(name, O_RDONLY);
  if (fd_ < 0) {
    LOG_ERROR << "Fail to open " << name << " due to " << "[" << errno << "]"
              << strerror(errno);
    return false;
  }

  void* data = mmap(NULL, state.st_size, PROT_READ, MAP_PRIVATE, fd_, 0);
  if (data == reinterpret_cast<void*>(-1)) {  // MAP_FAILED
    LOG_ERROR << "Fail to map to [" << fd_ << "]" << name << " due to " << "["
              << errno << "]" << strerror(errno);
    close(fd_);
    fd_ = -1;
    return false;
  }

  data_ = reinterpret_cast<char*>(data);
  CHECK(data_ != NULL);
  size_ = static_cast<int32>(state.st_size);
  return true;
}
}
