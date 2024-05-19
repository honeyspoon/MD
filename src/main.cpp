
#include <iostream>
#include <fstream>

#include "log.h"

#include "reader.h"
#include "parser.h"

#if defined(__linux__)
#include <endian.h>
// no need for macos
uint64_t ntohll(uint64_t n)
{
  return be64toh(n);
}
#endif

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

int system_events[MAX_STREAMS] = {0};
int accepted[MAX_STREAMS] = {0};
int replaced[MAX_STREAMS] = {0};
int executed[MAX_STREAMS] = {0};
int executed_shares[MAX_STREAMS] = {0};
int cancelled[MAX_STREAMS] = {0};

void reset_stats()
{
  memset(system_events, 0, MAX_STREAMS);
  memset(accepted, 0, MAX_STREAMS);
  memset(replaced, 0, MAX_STREAMS);
  memset(executed, 0, MAX_STREAMS);
  memset(executed_shares, 0, MAX_STREAMS);
  memset(cancelled, 0, MAX_STREAMS);
}

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

// not sure what the alternative to keeping them global is
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
using ouch::parser::stream_buffer_t;
void handleSystemEvent(stream_buffer_t &stream)
{
  system_events[stream.id]++;
  // Handle SYSTEM_EVENT
}

void handleAccepted(stream_buffer_t &stream)
{
  accepted[stream.id]++;
  // Handle ACCEPTED
}

void handleReplaced(stream_buffer_t &stream)
{
  replaced[stream.id]++;
  // Handle REPLACED
}

void handleCanceled(stream_buffer_t &stream)
{
  cancelled[stream.id]++;
  // Handle REPLACED
}

void handleExecuted(stream_buffer_t &stream)
{
  using namespace ouch;
  executed[stream.id]++;
  // Handle REPLACED
  auto *executed_msg = reinterpret_cast<executed_message_t *>(stream.buffer);
  executed_msg->executed_shares = ntohl(executed_msg->executed_shares);
  executed_shares[stream.id] += executed_msg->executed_shares;
}

std::fstream s("test.out", std::ios::out | std::ios::binary);

class Handler
{
};

void handler(stream_buffer_t &stream)
{
  using namespace ouch;
  auto *msg_header = reinterpret_cast<msg_header_t *>(stream.buffer);
  message_type_t msg_type = static_cast<message_type_t>(msg_header->message_type);
  switch (msg_type)
  {
  case SYSTEM_EVENT:
    handleSystemEvent(stream);
    break;
  case ACCEPTED:
    handleAccepted(stream);
    break;
  case EXECUTED:
    handleExecuted(stream);
    break;
  case REPLACED:
    handleReplaced(stream);
    break;
  case CANCELED:
    handleCanceled(stream);
    break;
  default:
    print("Unknown message type: ", msg_type);
  }
  s.write(reinterpret_cast<char *>(stream.buffer), msg_header->message_length + 2);
}

int main(int argc, char *argv[])
{
  args_t args = parse_args(argc, argv);

  using namespace ouch;

  reset_stats();

  auto reader = create_reader<CMappedFileReader>(args.file_name);
  // auto reader = create_reader<FileReader>(args.file_name);
  if (parser::parse(*reader, handler))
  {
    print("ERROR: analysis failed");
    return 1;
  }

  aggregate_stats();

  print("end");
}
