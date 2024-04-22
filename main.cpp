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

#if defined(__linux__)
#include <endian.h>
uint64_t ntohll(uint64_t n) {
  return be64toh(n);
}
#endif


void analyse(std::string file_name) {
  print("running analysis on file", file_name);
  FILE *file = fopen(file_name.c_str(), "rb");

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

    typedef struct {
      uint8_t buffer[BUFF_LEN];
      bool pending;
      uint16_t offset;
    } stream_buffer_t;

    stream_buffer_t stream_buffers[MAX_STREAMS];
    for (int i = 0; i < MAX_STREAMS; i++) {
      stream_buffers[i].pending = false;
      stream_buffers[i].offset = 0;
    }

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

      // read the data at the offset in the stream
      if (fread(buffer + stream->offset, header.packet_length, 1, file)) {
        stream->offset += header.packet_length;
      }

      outch_header_t *msg_header = (outch_header_t *)buffer;

      if (msg_header->message_length > MAX_MSG_LENGTH) {
        // no need to reconvert to little endian if it's already in the buffer
        msg_header->message_length = ntohs(msg_header->message_length);
        msg_header->timestamp = ntohll(msg_header->timestamp);
      }

      if (stream->offset < msg_header->message_length + 2) {
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
    int total_executed_shares = 0;
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
      total_system_events += system_events[i];
      print(" ", "Replaced", replaced[i]);
      total_replaced += replaced[i];
      print(" ", "Canceled", cancelled[i]);
      total_cancelled += cancelled[i];
      print(" ", "Executed", executed[i], "message | shares",
            executed_shares[i], "shares");
      total_executed += executed[i];
      total_executed_shares += executed_shares[i];
    }

    print("");
    print("Total");
    print(" ", "Accepted", total_accepted);
    print(" ", "System Event", total_system_events);
    print(" ", "Replaced", total_replaced);
    print(" ", "Cancelled", total_cancelled);
    print(" ", "Executed", total_executed, "message | shares",
          total_executed_shares);

    fclose(file);
  }
}

struct args_t {
  std::string file_name;
};

args_t parse_args(int argc, char *argv[]) {
  args_t args;

  if (argc < 2) {
    std::cerr << "Error: No filename provided" << std::endl;
    exit(1);
  }

  args.file_name = argv[1];

  return args;
}

int main(int argc, char *argv[]) {
  args_t args = parse_args(argc, argv);
  analyse(args.file_name);
}
