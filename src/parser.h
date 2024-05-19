#pragma once

#include "reader.h"

#if defined(__linux__)
#include <endian.h>
// no need for macos
uint64_t ntohll(uint64_t n) { return be64toh(n); }
#endif

namespace ouch {
namespace parser {

constexpr int BUFF_LEN = 128;
typedef struct {
  uint8_t id;
  uint16_t offset;
  uint8_t buffer[BUFF_LEN];
} stream_buffer_t;

using MessageHandler = void (*)(stream_buffer_t &);
template <typename T> int parse(Reader<T> &reader, MessageHandler handler);
} // namespace parser
} // namespace ouch
