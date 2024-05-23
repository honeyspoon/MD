#include <cstdint>

import ouch;
import ouch.parser;

import log;

import std;

import reader;

struct stats_t {
  int id;
  int system_events = 0;
  int accepted = 0;
  int replaced = 0;
  int executed = 0;
  int executed_shares = 0;
  int cancelled = 0;
};

void print_stats(const stats_t &stat) {
  println("");
  println(std::format("Stream {}", stat.id));
  println("- Accepted ", stat.accepted);
  println("- System Event ", stat.system_events);
  println("- Replaced ", stat.replaced);
  println("- Cancelled ", stat.cancelled);
  println("- Executed messages ", stat.executed, " | shares ",
          stat.executed_shares);
}

stats_t stats[ouch::MAX_STREAMS];

using namespace ouch::parser;
void handler(uint8_t stream_id, const ouch::msg_header_t *msg_header) {
  using ouch::msg_type_t;
  stats_t &stream_stats = stats[stream_id];
  stream_stats.id = stream_id;

  switch (msg_header->msg_type) {
  case msg_type_t::SYSTEM_EVENT:
    stream_stats.system_events++;
    break;
  case msg_type_t::ACCEPTED:
    stream_stats.accepted++;
    break;
  case msg_type_t::EXECUTED: {
    stream_stats.executed++;
    auto *executed_msg =
        reinterpret_cast<const ouch::executed_message_t *>(msg_header);
    stream_stats.executed_shares += ntohl(executed_msg->executed_shares);
    break;
  }
  case msg_type_t::REPLACED:
    stream_stats.replaced++;
    break;
  case msg_type_t::CANCELED:
    stream_stats.cancelled++;
    break;
  default:
    println("Unknown message type: ",
            static_cast<uint8_t>(msg_header->msg_type));
    break;
  }
}

struct args_t {
  std::string file_name;
};

args_t parse_args(int argc, char *argv[]) {
  args_t args;

  if (argc < 2) {
    std::cerr << "Error: No filename provided" << std::endl;
    std::exit(1);
  }

  args.file_name = argv[1];

  return args;
}

int main(int argc, char *argv[]) {
  args_t args = parse_args(argc, argv);

  println("Parsing ", args.file_name);
  CMappedFileReader reader{args.file_name};

  if (ouch::parser::parse(reader, handler)) {
    println("ERROR: analysis failed");
    return 1;
  }

  for (auto &stat : stats) {
    if (stat.accepted > 0) {
      print_stats(stat);
    }
  }

  return 0;
}
