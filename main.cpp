#include <cinttypes>
#include <ios>
#include <stdio.h>
#include <unistd.h>

#include "outch.h"
#include <arpa/inet.h>

#include <iostream>

template <typename T, typename... Args>
void print(const T &value, const Args &...args)
{
  std::cout << "" << value << " ";
  ((std::cout << args << " "), ...);
  std::cout << std::endl;
}

constexpr int MAX_STREAMS = 8;
constexpr int BUFF_LEN = 128;

#if defined(__linux__)
#include <endian.h>
uint64_t ntohll(uint64_t n)
{
  return be64toh(n);
}
#endif

// AoS instead of SoA for cache
// and array instead of hash
int system_events[MAX_STREAMS] = {0};
int accepted[MAX_STREAMS] = {0};
int replaced[MAX_STREAMS] = {0};
int executed[MAX_STREAMS] = {0};
int executed_shares[MAX_STREAMS] = {0};
int cancelled[MAX_STREAMS] = {0};

typedef struct
{
  uint16_t offset;
  uint8_t buffer[BUFF_LEN];
} stream_buffer_t;

void on_executed_msg(uint8_t *buffer, u_int8_t stream_id)
{
  executed_message_t *executed_msg = (executed_message_t *)buffer;
  executed_msg->executed_shares = ntohl(executed_msg->executed_shares);
  // TODO: convert other fields
  executed_shares[stream_id] += executed_msg->executed_shares;
}

int parse_msg(msg_header_t *msg_header, u_int8_t stream_id, uint8_t *buffer)
{
  switch (msg_header->message_type)
  {
  case SYSTEM_EVENT:
    system_events[stream_id]++;
    // TODO: handle
    break;
  case ACCEPTED:
    accepted[stream_id]++;
    // TODO: handle
    break;
  case REPLACED:
    replaced[stream_id]++;
    // TODO: handle
    break;
  case EXECUTED:
  {
    executed[stream_id]++;
    on_executed_msg(buffer, stream_id);
    break;
  }
  case CANCELED:
    cancelled[stream_id]++;
    // TODO: handle
    break;
  default:
    print("ERROR: unkown message type!", char(msg_header->message_type));
    return 1;
  }
  return 0;
}

// this struct does not have to be performant since it's used once at the end
struct stats_t
{
  std::string name;
  int system_events = 0;
  int accepted = 0;
  int replaced = 0;
  int executed = 0;
  int executed_shares = 0;
  int cancelled = 0;
};

void print_stats(const stats_t &stats)
{
  print("");
  print(stats.name);
  print(" ", "Accepted", stats.accepted);
  print(" ", "System Event", stats.system_events);
  print(" ", "Replaced", stats.replaced);
  print(" ", "Cancelled", stats.cancelled);
  print(" ", "Executed", stats.executed, "messages | shares", stats.executed_shares);
}

void aggregate_stats()
{
  stats_t stats[MAX_STREAMS] = {};
  for (int i = 0; i < MAX_STREAMS; i++)
  {
    stats[i] = {
        "Stream " + std::to_string(i),
        system_events[i],
        accepted[i],
        replaced[i],
        executed[i],
        executed_shares[i],
        cancelled[i]};
  }

  // aggregate total stats
  stats_t total_stats = {"Total"};
  for (int i = 0; i < MAX_STREAMS; i++)
  {
    total_stats.system_events += system_events[i];
    total_stats.accepted += accepted[i];
    total_stats.replaced += replaced[i];
    total_stats.executed += executed[i];
    total_stats.executed_shares += executed_shares[i];
    total_stats.cancelled += cancelled[i];
  }

  // output
  for (int i = 0; i < MAX_STREAMS; i++)
  {
    if (stats[i].accepted)
      print_stats(stats[i]);
  }
  print_stats(total_stats);
}

int read_packet_header(FILE *file, packet_header_t *header)
{
  int res = fread(header, sizeof(packet_header_t), 1, file);

  // big to little endian conversion
  header->stream_id = ntohs(header->stream_id);
  header->packet_length = ntohl(header->packet_length);

  if (ferror(file))
  {
    print("File read error!");
    return 0;
  }

  if (feof(file))
  {
    print("Unexpected end of file!");
    return 0;
  }

  return res;
}

void analyse(std::string file_name)
{

  stream_buffer_t stream_buffers[MAX_STREAMS];
  for (int i = 0; i < MAX_STREAMS; i++)
  {
    stream_buffers[i].offset = 0;
  }

  print("running analysis on file", file_name);
  FILE *file = fopen(file_name.c_str(), "rb");

  if (file)
  {
    packet_header_t header;

    while (read_packet_header(file, &header) == 1)
    {
      stream_buffer_t *stream = &stream_buffers[header.stream_id];
      uint8_t *buffer = stream->buffer;

      // read the data at the offset in the stream
      if (fread(buffer + stream->offset, header.packet_length, 1, file))
      {
        stream->offset += header.packet_length;
      }

      msg_header_t *msg_header = (msg_header_t *)buffer;

      if (msg_header->message_length > MAX_MSG_LENGTH)
      {
        // no need to reconvert to little endian if it's already in the buffer
        msg_header->message_length = ntohs(msg_header->message_length);
        msg_header->timestamp = ntohll(msg_header->timestamp);
      }

      // incomplete message
      if (stream->offset != msg_header->message_length + 2)
      {
        continue;
      }

      // we have a complete message and we can reset
      stream_buffers[header.stream_id].offset = 0;

      if (parse_msg(msg_header, header.stream_id, buffer))
      {
        print("ERROR: parsing message");
      }
    }
    fclose(file);

    aggregate_stats();
  }
}

struct args_t
{
  std::string file_name;
};

args_t parse_args(int argc, char *argv[])
{
  args_t args;

  if (argc < 2)
  {
    std::cerr << "Error: No filename provided" << std::endl;
    exit(1);
  }

  args.file_name = argv[1];

  return args;
}

int main(int argc, char *argv[])
{
  args_t args = parse_args(argc, argv);
  analyse(args.file_name);
}
