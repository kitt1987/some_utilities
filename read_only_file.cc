#include "file.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

namespace sus {
ReadOnlyFile::ReadOnlyFile()
    : fd_(-1) {
}

ReadOnlyFile::ReadOnlyFile(std::string const& name)
    : fd_(-1) {
  CHECK(TryToOpen(name)) << name;
}

ReadOnlyFile::ReadOnlyFile(char const* name)
    : fd_(-1) {
  CHECK(TryToOpen(name)) << name;
}

ReadOnlyFile::~ReadOnlyFile() {
  if (fd_ > -1)
    close(fd_);
}

bool ReadOnlyFile::TryToOpen(char const* name) {
  CHECK_EQ(fd_, -1)<< name;
  fd_ = open(name, O_RDONLY);
  if (fd_ < 0) {
    LOG_ERROR<< "Fail to open " << name << " due to [" << errno << "]"
    << strerror(errno);
    return false;
  }

  return true;
}

int32 ReadOnlyFile::Read(std::string* data) {
  CHECK_GT(fd_, -1);
  CHECK_GT(data->size(), 0);
  ssize_t result = read(fd_, const_cast<char*>(data->data()), data->size());
  if (result < 0) {
    LOG_ERROR << "Fail to read from file " << fd_ << " due to [" << errno << "]"
              << strerror(errno);
  }

  return static_cast<int32>(result);
}

int32 ReadOnlyFile::Read(int32 offset, std::string* data) {
  CHECK_GT(fd_, -1);
  if (lseek(fd_, offset, SEEK_SET) == -1) {
    LOG_ERROR << "Fail to seek file " << fd_ << " due to [" << errno << "]"
              << strerror(errno);
    return -1;
  }

  return Read(data);
}

int32 ReadOnlyFile::Size() {
  CHECK_GT(fd_, -1);
  struct stat state;
  bzero(&state, sizeof state);

  CHECK_EQ(fstat(fd_, &state), 0)<< "Fail to stat file " << fd_ << " due to "
  << "[" << errno << "]" << strerror(errno);
  return static_cast<int32>(state.st_size);
}
}
