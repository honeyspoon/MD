#include <cxxopts.hpp>

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

int main(int argc, char *argv[]) {
  cxxopts::Options options("list symbols", "list symbols in ouch file");

  options.add_options()("i,input_file", "Input file name",
                        cxxopts::value<std::string>());

  auto result = options.parse(argc, argv);
  auto input_file = result["input_file"].as<std::string>();

  if (input_file == "-") {
    mlog::info("Parsing OUCH file from STDIN");
    StreamReader reader{std::cin};
    if (parser::parse(reader, handler)) {
      mlog::error("error parsing file");
      return 1;
    }
  } else {
    mlog::info("Parsing OUCH file: {}", input_file);
    FileReader reader{input_file};
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
