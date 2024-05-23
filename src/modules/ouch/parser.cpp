module;
#include <cstdint>
export module ouch.parser;
import std;

import ouch;
import reader;
import log;

namespace ouch {
namespace parser {

#if defined(__linux__)
#include <endian.h>
// no need for macos
uint64_t ntohll(uint64_t n) { return be64toh(n); }
#endif

constexpr int BUFF_LEN = 128;
export typedef struct {
  uint8_t id;
  uint8_t *bp;
  uint8_t buffer[BUFF_LEN];
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
concept Callable = requires(T t, stream_buffer_t &stream) {
  { t(stream) } -> std::same_as<void>;
};

export int parse(Readable auto &reader, Callable auto &&handler) {
  stream_buffer_t stream_buffers[MAX_STREAMS];
  for (int i = 0; i < MAX_STREAMS; i++) {
    stream_buffers[i].bp = stream_buffers[i].buffer;
    stream_buffers[i].id = i;
  }

  while (!reader.error() && !reader.eof()) {
    packet_header_t header;
    reader.read(reinterpret_cast<uint8_t *>(&header), sizeof(packet_header_t));
    auto stream_id = ntohs(header.stream_id);
    auto packet_length = ntohl(header.packet_length);

    stream_buffer_t &stream = stream_buffers[stream_id];

    reader.read(stream.bp, packet_length);

    stream.bp += packet_length;

    if (!is_complete(stream))
      continue;

    msg_header_t *msg_header = reinterpret_cast<msg_header_t *>(stream.bp);

    handler(stream);
  }
  println("ddddddddd");
  println("End of file reached.");

  return 0;
}

} // namespace parser
}; // namespace ouch
