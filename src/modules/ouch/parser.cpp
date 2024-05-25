module;
#include "spdlog/spdlog.h"
#include <array>
#include <cstdint>
#include <iostream>

#include <algorithm>
#include <climits>
#include <concepts>
#include <cstdint>
#include <stddef.h>

export module ouch.parser;

import ouch;
import reader;

namespace ouch {
namespace parser {

#if defined(__linux__)
#include <endian.h>
// no need for macos
uint64_t ntohll(uint64_t n) { return be64toh(n); }
#endif

constexpr int BUFF_LEN = 128;

export typedef struct {
  unsigned int id;
  std::byte *bp;
  std::byte buffer[BUFF_LEN];
} stream_buffer_t;

bool is_complete(stream_buffer_t &stream) {
  auto *msg_header = reinterpret_cast<msg_header_t *>(stream.buffer);
  auto message_length = ntohs(msg_header->message_length);

  std::ptrdiff_t buff_size = stream.bp - stream.buffer;
  if (buff_size != message_length + sizeof(uint16_t)) {
    return false;
  }

  stream.bp = stream.buffer;
  return true;
}

template <typename T>
concept Callable =
    requires(T t, const unsigned int id, msg_header_t *msg_header) {
      { t(id, msg_header) } -> std::same_as<void>;
    };

export int parse(Readable auto &reader, Callable auto &&handler) {
  std::array<stream_buffer_t, MAX_STREAMS> stream_buffers;
  for (int i = 0; i < MAX_STREAMS; i++) {
    stream_buffers[i].bp = stream_buffers[i].buffer;
    stream_buffers[i].id = i;
  }

  while (!reader.error() && !reader.eof()) {
    packet_header_t header{.stream_id = 0, .packet_length = 0};
    reader.read(reinterpret_cast<std::byte *>(&header),
                sizeof(packet_header_t));

    hn_swap(header);

    stream_buffer_t &stream = stream_buffers[header.stream_id];

    reader.read(stream.bp, header.packet_length);
    stream.bp += header.packet_length;

    if (!is_complete(stream))
      continue;

    // not const because the handler can change endianness
    msg_header_t *msg_header = reinterpret_cast<msg_header_t *>(stream.buffer);
    handler(header.stream_id, msg_header);
  }

  spdlog::info("End of file reached.");

  return 0;
}

} // namespace parser
}; // namespace ouch
