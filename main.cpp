#include <cinttypes>
#include <ios>
#include <stdio.h>
#include <unistd.h>

#include "outch.h"
#include <arpa/inet.h>

#include <iostream>

template <typename T, typename... Args>
void print(const T &value, const Args &...args) {
  std::cout << "" << value << " ";
  ((std::cout << args << " "), ...);
  std::cout << std::endl;
}

constexpr int MAX_STREAMS = 8;
constexpr int BUFF_LEN = 128;

typedef struct {
  uint8_t buffer[BUFF_LEN];
  bool pending;
  uint16_t offset;
} stream_buffer_t;

int main() {
  FILE *file = fopen("OUCHLMM2.incoming.packets", "rb");

  if (file) {
    packet_header_t header;

    // AoS instead of SoA for cache
    // and array instead of hash
    int system_events[MAX_STREAMS] = {0};
    int accepted[MAX_STREAMS] = {0};
    int replaced[MAX_STREAMS] = {0};
    int executed[MAX_STREAMS] = {0};
    int executed_shares[MAX_STREAMS] = {0};
    int cancelled[MAX_STREAMS] = {0};

    stream_buffer_t stream_buffers[MAX_STREAMS];

    while (fread(&header, sizeof(header), 1, file) == 1) {
      // big to little endian conversion
      header.stream_id = ntohs(header.stream_id);
      header.packet_length = ntohl(header.packet_length);

      if (ferror(file)) {
        print("File read error!");
        break;
      }

      if (feof(file)) {
        print("Unexpected end of file!");
        break;
      }

      stream_buffer_t *stream = &stream_buffers[header.stream_id];
      uint8_t *buffer = stream->buffer;
      if (fread(buffer + stream->offset, header.packet_length, 1, file)) {
        stream->offset += header.packet_length;
      }

      outch_header_t *msg_header = (outch_header_t *)buffer;

      if (msg_header->message_length > 80) {
        // no need to reconvert to little endian if it's already in the buffer
        msg_header->message_length = ntohs(msg_header->message_length);
        msg_header->timestamp = ntohll(msg_header->timestamp);
      }

      if (stream->offset - 2 < msg_header->message_length) {
        stream_buffers[header.stream_id].pending = true;
        continue;
      } else {
        stream_buffers[header.stream_id].pending = false;
        stream_buffers[header.stream_id].offset = 0;
      }

      switch (msg_header->message_type) {
      case SYSTEM_EVENT:
        system_events[header.stream_id]++;
        break;
      case ACCEPTED:
        accepted[header.stream_id]++;
        break;
      case REPLACED:
        replaced[header.stream_id]++;
        break;
      case EXECUTED: {
        executed[header.stream_id]++;
        executed_message_t *executed_msg = (executed_message_t *)buffer;
        executed_shares[header.stream_id] +=
            ntohl(executed_msg->executed_shares);
        break;
      }
      case CANCELED:
        cancelled[header.stream_id]++;
        break;
      default:
        print("ERROR: unkown message type!", char(msg_header->message_type));
        break;
      }
    }

    int total_system_events = 0;
    int total_accepted = 0;
    int total_replaced = 0;
    int total_executed = 0;
    int total_cancelled = 0;

    for (int i = 0; i < MAX_STREAMS; i++) {
      if (accepted[i] == 0) {
        break;
      }
      print("");
      print("Stream", i);
      print(" ", "Accepted", accepted[i]);
      total_accepted += accepted[i];
      print(" ", "System Event", system_events[i]);
      total_system_events += accepted[i];
      print(" ", "Replaced", replaced[i]);
      total_replaced += replaced[i];
      print(" ", "Executed", executed[i], "message shares", executed_shares[i],
            "shares");
      total_executed += executed[i];
      print(" ", "Canceled", cancelled[i]);
      total_cancelled += cancelled[i];
    }

    print("");
    print("Total");
    print(" ", "Accepted", total_accepted);
    print(" ", "System Event", total_system_events);
    print(" ", "Replaced", total_replaced);
    print(" ", "Executed", total_executed);
    print(" ", "Cancelled", total_cancelled);

    fclose(file);
  }

  return 0;
}
