#pragma once
#include "base.h"
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/statvfs.h>
#include <errno.h>
#include <string.h>
#include <uuid/uuid.h>

namespace sus {
template<typename Integer>
Integer Text2(char const* text) {
  CHECK(text != NULL);
  uint32 length = static_cast<uint32>(strlen(text));
  CHECK_GT(length, 0U);
  CHECK_NE(text[0], '-');
  int32 index = static_cast<int32>(length - 1);
  Integer factor = 1;
  Integer integer = 0;
  while (index >= 0) {
    integer += ((text[index] - '0') * factor);
    factor *= 10;
    --index;
  }

  return integer;
}

template<typename Integer>
Integer Text2(std::string const& text) {
  CHECK_GT(text.length(), 0U);
  CHECK_NE(text[0], '-');
  int index = static_cast<int>(text.length() - 1);
  Integer factor = 1;
  Integer integer = 0;
  while (index >= 0) {
    integer += ((text[index] - '0') * factor);
    factor *= 10;
    --index;
  }

  return integer;
}

template<typename Integer>
Integer Hex2(std::string const& text, std::string::size_type pos) {
  CHECK_GT(text.length(), 0U);
  CHECK_NE(text[0], '-');
  std::string::size_type index = static_cast<int>(text.length() - 1);
  Integer factor = 1;
  Integer integer = 0;
  while (index >= pos) {
    char c = text[index];
    if (c >= 'a') {
      integer += ((text[index] - 'a' + '\012') * factor);
    } else if (c >= 'A') {
      integer += ((text[index] - 'A' + '\012') * factor);
    } else {
      integer += ((text[index] - '0') * factor);
    }

    factor *= 16;
    --index;
  }

  return integer;
}

// CAUTION number_system could be 2,8,10,16
template<typename Integer>
std::string ConvertToStringFrom(Integer integer, int const number_system) {
  std::string text;
  if (integer == 0)
    return "0";

  CHECK(
      number_system == 2 || number_system == 8 || number_system == 10
          || number_system == 16);

  bool minus = integer < Integer();
  if (minus)
    integer = static_cast<Integer>(~(integer - 1));

  while (integer > 0) {
    Integer mod = integer % static_cast<Integer>(number_system);
    if (mod < 10) {
      text.push_back(static_cast<char>('0' + mod));
    } else {
      text.push_back(static_cast<char>('A' + mod - 10));
    }

    integer /= static_cast<Integer>(number_system);
  }

  if (minus)
    text.push_back('-');

  std::reverse(text.begin(), text.end());
  return text;
}

inline std::string HexDump(int const* binary, size_t length) {
  std::string out;
  for (size_t i = 0; i < length; ++i) {
    out.append("0x");
    out.append(ConvertToStringFrom(binary[i], 16));
    out.push_back(',');
  }

  if (!out.empty())
    out.pop_back();

  return out;
}

inline int32 PtoN(std::string const& ip) {
  int32 ip_decimal;
  CHECK_EQ(inet_pton(AF_INET, ip.data(), &ip_decimal), 1)<< ip;
  return ip_decimal;
}

inline int64 GetCurrentTimeInMS() {
  struct timeval tv;
  bzero(&tv, sizeof(tv));
  CHECK_EQ(gettimeofday(&tv, NULL), 0)<< "Fail to get current time due to "
  << "[" << errno << "]"
  << strerror(errno);
  return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

inline std::string GenerateUUID() {
  uuid_t uuid;
  uuid_generate(uuid);
  std::string uuid_text;
  int bytes_uuid = static_cast<int>(sizeof uuid);
  for (int i = 0; i < bytes_uuid; ++i) {
    uuid_text.append(ConvertToStringFrom<unsigned char>(uuid[i], 16));
  }

  return uuid_text;
}

template<typename Container>
void ClearHeapElemsInSequentialContainer(Container* container) {
  if (container->empty())
    return;

  for (typename Container::iterator each = container->begin();
      each != container->end(); ++each) {
    delete *each;
  }

  container->clear();
}

template<typename Container>
void ClearHeapElemsInAssociativeContainer(Container* container) {
  if (container->empty())
    return;

  for (typename Container::iterator each = container->begin();
      each != container->end(); ++each) {
    delete each->second;
  }

  container->clear();
}
}
