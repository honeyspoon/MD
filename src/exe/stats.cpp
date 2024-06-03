import std;

import args;
import mlog;

import json;
import ouch;
import ouch.parser;

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
  std::cout << "- Executed messages " << stat.executed << " | shares " << stat.executed_shares
            << std::endl;
}

void print_stats_json(const stats_t &stat) {
  auto out = json{
      {"stream_id", stat.id},
      {"accepted", stat.accepted},
      {"system_events", stat.system_events},
      {"replaced", stat.replaced},
      {"cancelled", stat.cancelled},
      {"executed", stat.executed},
      {"executed_shares", stat.executed_shares},
  };
  std::cout << out << std::endl;
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
    stream_stats.executed_shares += hn_swap_copy(executed_msg->executed_shares);
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
    mlog::warn("Unknown message type {}", static_cast<char>(msg_header->msg_type));
    break;
  }
  }
}

int main(int argc, char *argv[]) {
  cxxopts::Options options("stats", "get stats for a stream");

  options.add_options() //
      ("i,input_file", "Input file name",
       cxxopts::value<std::string>()->default_value("-")) //
      ("j,json", "Json output",
       cxxopts::value<bool>()->default_value("false")) //
      ;

  auto result = options.parse(argc, argv);
  auto input_file = result["input_file"].as<std::string>();

  std::variant<StreamReader, FileReader> reader = StreamReader{std::cin};
  if (input_file != "-") {
    if (!std::filesystem::exists(input_file)) {
      mlog::error("File {} does not exist", input_file);
      std::exit(1);
    }
    mlog::info("Parsing {}", input_file);
    reader = FileReader{std::string(input_file)};
  }

  bool error = std::visit([](auto &&r) -> bool { return parser::parse(r, handler); }, reader);

  if (error) {
    mlog::error("error parsing file");
    return 1;
  }

  auto json_output = result["json"].as<bool>();
  for (auto &stat : stats) {
    if (stat.accepted > 0) {
      if (json_output) {
        print_stats_json(stat);
      } else {
        print_stats(stat);
      }
    }
  }

  return 0;
}
