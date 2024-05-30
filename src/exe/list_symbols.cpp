import std;
import mlog;

import ouch;
import ouch.parser;
import reader;

using namespace ouch;
std::set<symbol_t> symbols;

void handler(const stream_id_t, msg_header_t *msg_header) {
  switch (msg_header->msg_type) {
    case msg_type_t::SYSTEM_EVENT:
      break;
    case msg_type_t::ACCEPTED: {
      auto *accepted = std::bit_cast<accepted_message_t *>(msg_header);
      symbols.emplace(accepted->symbol);
      break;
    }
    case msg_type_t::EXECUTED:
      break;
    case msg_type_t::REPLACED:
      break;
    case msg_type_t::CANCELED:
      break;
    default:
      mlog::warn("Unknown message type {}",
                 static_cast<char>(msg_header->msg_type));
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
  if (args.file_name == "-") {
    mlog::info("Parsing OUCH file from STDIN");
    StreamReader reader{std::cin};
    if (parser::parse(reader, handler)) {
      mlog::error("error parsing file");
      return 1;
    }
  } else {
    mlog::info("Parsing OUCH file: {}", args.file_name);
    FileReader reader{args.file_name};
    if (parser::parse(reader, handler)) {
      mlog::error("error parsing file");
      return 1;
    }
  }

  for (auto symbol : symbols) {
    std::cout << to_string(symbol) << std::endl;
  }

  mlog::info("done");

  return 0;
}
