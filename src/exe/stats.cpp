#include <cxxopts.hpp>

import std;
import ouch;
import ouch.parser;
import mlog;

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
  std::cout << std::endl;
  std::cout << "Stream " << stat.id << std::endl;
  std::cout << "- Accepted " << stat.accepted << std::endl;
  std::cout << "- System Event " << stat.system_events << std::endl;
  std::cout << "- Replaced " << stat.replaced << std::endl;
  std::cout << "- Cancelled " << stat.cancelled << std::endl;
  std::cout << "- Executed messages " << stat.executed << " | shares "
            << stat.executed_shares << std::endl;
}

using namespace ouch;
stats_t stats[MAX_STREAMS];

void handler(stream_id_t stream_id, msg_header_t *msg_header) {
  stats_t &stream_stats = stats[stream_id];
  stream_stats.id = stream_id;

  switch (msg_header->msg_type) {
    case msg_type_t::SYSTEM_EVENT: {
      stream_stats.system_events++;
      break;
    }
    case msg_type_t::ACCEPTED: {
      stream_stats.accepted++;
      break;
    }
    case msg_type_t::EXECUTED: {
      stream_stats.executed++;
      auto *executed_msg = std::bit_cast<executed_message_t *>(msg_header);
      stream_stats.executed_shares +=
          hn_swap_copy(executed_msg->executed_shares);
      break;
    }
    case msg_type_t::REPLACED: {
      stream_stats.replaced++;
      break;
    }
    case msg_type_t::CANCELED: {
      stream_stats.cancelled++;
      break;
    }
    default: {
      mlog::warn("Unknown message type {}",
                 static_cast<char>(msg_header->msg_type));
      break;
    }
  }
}

int main(int argc, char *argv[]) {
  cxxopts::Options options("stats", "get stats for a stream");

  options.add_options()("i,input_file", "Input file name",
                        cxxopts::value<std::string>());

  auto result = options.parse(argc, argv);
  auto input_file = result["input_file"].as<std::string>();

  mlog::info("Parsing OUCH file: {}", input_file);
  CMappedFileReader reader{input_file};

  if (parser::parse(reader, handler)) {
    mlog::error("error parsing file");
    return 1;
  }

  for (auto &stat : stats) {
    if (stat.accepted > 0) {
      print_stats(stat);
    }
  }

  return 0;
}
