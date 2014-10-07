#pragma once
#include "base.h"

namespace sus {
class TinyReadOnlyFile {
 public:
  TinyReadOnlyFile();
  explicit TinyReadOnlyFile(std::string const& name);
  explicit TinyReadOnlyFile(char const* name);
  virtual ~TinyReadOnlyFile();

  bool TryToOpen(std::string const& name) {
    return TryToOpen(name.c_str());
  }

  bool TryToOpen(char const* name);
  char const* Read() const {
    return data_;
  }

  int32 Size() {
    return size_;
  }

 private:
  int fd_;
  char* data_;
  int32 size_;

  DISALLOW_COPY_AND_ASSIGN(TinyReadOnlyFile);
};

class ReadOnlyFile {
 public:
  ReadOnlyFile();
  ReadOnlyFile(std::string const& name);
  ReadOnlyFile(char const* name);
  virtual ~ReadOnlyFile();

  bool TryToOpen(std::string const& name) {
    return TryToOpen(name.c_str());
  }

  bool TryToOpen(char const* name);

  int32 Read(std::string* data);
  int32 Read(int32 offset, std::string* data);
  int32 Size();

 private:
  int fd_;

  DISALLOW_COPY_AND_ASSIGN(ReadOnlyFile);
};

class WriteOnlyFile {
 public:
  WriteOnlyFile();
  explicit WriteOnlyFile(std::string const& name);
  explicit WriteOnlyFile(char const* name);
  virtual ~WriteOnlyFile();

  bool TryToOpen(std::string const& name) {
    return TryToOpen(name.c_str());
  }

  bool TryToOpen(char const* name);

  int32 Write(std::string const& data, int32 data_offset);
  int32 Write(std::string const& data) {
    return Write(data, 0);
  }

  int32 Write(int32 offset, std::string const& data, int32 data_offset);
  int32 Write(int32 offset, std::string const& data) {
    return Write(offset, data, 0);
  }

  int32 Size();
  bool Failed() const {
    return failed_;
  }

 private:
  int fd_;
  bool failed_;

  DISALLOW_COPY_AND_ASSIGN(WriteOnlyFile);
};

class RandomFile {
 public:
  RandomFile();
  explicit RandomFile(std::string const& name);
  explicit RandomFile(char const* name);
  virtual ~RandomFile();

  bool TryToOpen(std::string const& name) {
    return TryToOpen(name.c_str());
  }

  bool TryToOpen(char const* name);

  int32 Write(std::string const& data, int32 data_offset);
  int32 Write(std::string const& data) {
    return Write(data, 0);
  }

  int32 Write(int32 offset, std::string const& data, int32 data_offset);
  int32 Write(int32 offset, std::string const& data) {
    return Write(offset, data, 0);
  }

  int32 Size();
  bool Failed() const {
    return failed_;
  }

 private:
  int fd_;
  bool failed_;

  DISALLOW_COPY_AND_ASSIGN(RandomFile);
};
}
