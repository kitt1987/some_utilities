#include "file.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

namespace sus {
WriteOnlyFile::WriteOnlyFile()
    : fd_(-1),
      failed_(false) {
}

WriteOnlyFile::WriteOnlyFile(std::string const& name)
    : fd_(-1),
      failed_(false) {
  CHECK(TryToOpen(name)) << name;
}

WriteOnlyFile::WriteOnlyFile(char const* name)
    : fd_(-1),
      failed_(false) {
  CHECK(TryToOpen(name)) << name;
}

WriteOnlyFile::~WriteOnlyFile() {
  if (fd_ > -1) {
    fsync(fd_);
    close(fd_);
  }
}

bool WriteOnlyFile::TryToOpen(char const * name) {
  CHECK_EQ(fd_, -1)<< name;
  fd_ = open(name, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
  if (fd_ < 0) {
    LOG_ERROR<< "Fail to open " << name << " due to [" << errno << "]"
    << strerror(errno);
    return false;
  }

  return true;
}

int32 WriteOnlyFile::Write(std::string const& data, int32 data_offset) {
  CHECK_GT(fd_, -1);
  CHECK_GT(data.size(), 0);
  ssize_t result = write(fd_, data.data() + data_offset,
                         data.size() - data_offset);
  if (result < 0) {
    LOG_ERROR << "Fail to write to file " << fd_ << " due to [" << errno << "]"
              << strerror(errno);
    failed_ = true;
  }

  return static_cast<int32>(result);
}

int32 WriteOnlyFile::Write(int32 offset, std::string const& data,
                           int32 data_offset) {
  CHECK_GT(fd_, -1);
  if (lseek(fd_, offset, SEEK_SET) == -1) {
    LOG_ERROR << "Fail to seek file " << fd_ << " due to [" << errno << "]"
              << strerror(errno);
    failed_ = true;
    return -1;
  }

  return Write(data, data_offset);
}

int32 WriteOnlyFile::Size() {
  CHECK_GT(fd_, -1);
  struct stat state;
  bzero(&state, sizeof state);

  CHECK_EQ(fstat(fd_, &state), 0)<< "Fail to stat file " << fd_ << " due to "
  << "[" << errno << "]" << strerror(errno);
  return static_cast<int32>(state.st_size);
}
}
