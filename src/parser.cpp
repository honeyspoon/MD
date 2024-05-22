module;

// #include <print>

#include "ouch.h"

import reader;

export module ouch.parser;

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
  uint16_t offset;
  uint8_t buffer[BUFF_LEN];
} stream_buffer_t;

bool is_complete(stream_buffer_t &stream) {
  auto *msg_header = reinterpret_cast<msg_header_t *>(stream.buffer);

  if (msg_header->message_length > MAX_MSG_LENGTH) {
    // no need to reconvert to little endian if it's already in the buffer
    msg_header->message_length = ntohs(msg_header->message_length);
    msg_header->timestamp = ntohll(msg_header->timestamp);
  }

  // incomplete message
  if (stream.offset != msg_header->message_length + 2) {
    return false;
  }
  stream.offset = 0;
  return true;
}

bool read_packet_header(Readable auto &reader, packet_header_t *header) {
  // std::println("read_packet_header");
  reader.read(reinterpret_cast<char *>(header), sizeof(packet_header_t));

  // big to little endian conversion
  header->stream_id = ntohs(header->stream_id);
  header->packet_length = ntohl(header->packet_length);
  // std::println("packet {}  | lenght {}", header->stream_id,
  //              header->packet_length);

  // std::println("eof peteti {}", reader.eof());
  if (reader.eof()) {
    return false;
  }

  if (reader.error()) {
    // std::println("fail");
    // reader.print_error();
    return false;
  }

  return true;
}

bool read_msg(Readable auto &reader, stream_buffer_t &stream,
              const uint32_t packet_length) {
  if (packet_length > sizeof(stream.buffer) - stream.offset) {
    // std::println("Packet length exceeds buffer capacity!");
    return 1;
  }

  reader.read(reinterpret_cast<char *>(stream.buffer + stream.offset),
              packet_length);

  if (reader.error()) {
    // reader.print_error();
    // std::println("File read error!");
    return false;
  }

  if (reader.eof()) {
    // std::println("End of file reached before completing packet read!");
    return false;
  }

  stream.offset += packet_length;
  return true;
}

using MessageHandler = void (*)(stream_buffer_t &);

export int parse(Readable auto &reader, MessageHandler) {
  // export template <Readable T> int parse(Reader<T> &reader, MessageHandler) {
  stream_buffer_t stream_buffers[MAX_STREAMS];
  for (int i = 0; i < MAX_STREAMS; i++) {
    stream_buffers[i].offset = 0;
    stream_buffers[i].id = i;
  }

  packet_header_t header;

  // std::println("b");
  while (read_packet_header(reader, &header)) {
    // std::println("c");
    stream_buffer_t &stream = stream_buffers[header.stream_id];

    if (!read_msg(reader, stream, header.packet_length)) {
      // std::println("ERROR: reading message");
      return 1;
    }

    if (!is_complete(stream))
      continue;

    // handler(stream);
  }

  // std::println("End of file reached.");
  return 0;
}

} // namespace parser
}; // namespace ouch
