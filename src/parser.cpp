#include <print>

#include "ouch.h"
#include "parser.h"

namespace ouch {
namespace parser {

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

template <typename T>
bool read_packet_header(Reader<T> &reader, packet_header_t *header) {
  std::println("read_packet_header");
  reader.read(reinterpret_cast<char *>(header), sizeof(packet_header_t));

  // big to little endian conversion
  header->stream_id = ntohs(header->stream_id);
  header->packet_length = ntohl(header->packet_length);
  std::println("packet {}  | lenght {}", header->stream_id,
               header->packet_length);

  std::println("eof peteti {}", reader.eof());
  if (reader.eof()) {
    return false;
  }

  if (reader.error()) {
    std::println("fail");
    reader.print_error();
    return false;
  }

  return true;
}

template <typename T>
bool read_msg(Reader<T> &reader, stream_buffer_t &stream,
              const uint32_t packet_length) {
  if (packet_length > sizeof(stream.buffer) - stream.offset) {
    std::println("Packet length exceeds buffer capacity!");
    return 1;
  }

  reader.read(reinterpret_cast<char *>(stream.buffer + stream.offset),
              packet_length);

  if (reader.error()) {
    reader.print_error();
    std::println("File read error!");
    return false;
  }

  if (reader.eof()) {
    std::println("End of file reached before completing packet read!");
    return false;
  }

  stream.offset += packet_length;
  return true;
}

template <typename T> int parse(Reader<T> &reader, MessageHandler) {
  std::println("a");
  stream_buffer_t stream_buffers[MAX_STREAMS];
  for (int i = 0; i < MAX_STREAMS; i++) {
    stream_buffers[i].offset = 0;
    stream_buffers[i].id = i;
  }

  packet_header_t header;

  std::println("b");
  while (read_packet_header(reader, &header)) {
    std::println("c");
    stream_buffer_t &stream = stream_buffers[header.stream_id];

    if (!read_msg(reader, stream, header.packet_length)) {
      std::println("ERROR: reading message");
      return 1;
    }

    if (!is_complete(stream))
      continue;

    // handler(stream);
  }

  std::println("End of file reached.");
  return 0;
}

template int parse(Reader<FileReader> &reader, MessageHandler handler);
template int parse(Reader<CFileReader> &reader, MessageHandler handler);
template int parse(Reader<CMappedFileReader> &reader, MessageHandler handler);

} // namespace parser
}; // namespace ouch
