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

constexpr int BUFF_LEN = 128;

export typedef struct {
  unsigned int id;
  std::byte *bp;
  std::array<std::byte, BUFF_LEN> buffer;
} stream_t;

bool is_complete(stream_t &stream) {
  auto *msg_header = std::bit_cast<msg_header_t *>(stream.buffer.data());
  auto message_length = msg_header->message_length;
  hn_swap(message_length);

  std::ptrdiff_t buff_size = stream.bp - stream.buffer.data();
  if (buff_size != message_length + sizeof(uint16_t)) {
    return false;
  }

  stream.bp = stream.buffer.data();
  return true;
}

template <typename T>
concept Callable =
    requires(T t, const unsigned int id, msg_header_t *msg_header) {
      { t(id, msg_header) } -> std::same_as<void>;
    };

export int parse(Readable auto &reader, Callable auto &&handler) {
  std::array<stream_t, MAX_STREAMS> streams;
  for (int i = 0; i < MAX_STREAMS; i++) {
    streams[i].bp = streams[i].buffer.data();
    streams[i].id = i;
  }

  while (!reader.error() && !reader.eof()) {
    packet_header_t header{.stream_id = 0, .packet_length = 0};
    reader.read(std::bit_cast<std::byte *>(&header), sizeof(packet_header_t));

    hn_swap(header);

    stream_t &stream = streams[header.stream_id];

    reader.read(stream.bp, header.packet_length);
    stream.bp += header.packet_length;

    if (!is_complete(stream))
      continue;

    // not const because the handler can change endianness
    msg_header_t *msg_header =
        std::bit_cast<msg_header_t *>(stream.buffer.data());
    handler(header.stream_id, msg_header);
  }

  spdlog::info("End of file reached.");

  return 0;
}

} // namespace parser
}; // namespace ouch
