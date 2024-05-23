#include "spdlog/spdlog.h"

#include <cstdint>
#include <iostream>
#include <string>

import ouch;
import ouch.parser;

import reader;

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

using namespace ouch::parser;
void handler(uint8_t, const ouch::msg_header_t *msg_header) {
  using ouch::msg_type_t;
  switch (msg_header->msg_type) {
  case msg_type_t::SYSTEM_EVENT:
    break;
  case msg_type_t::ACCEPTED:
    break;
  case msg_type_t::EXECUTED: {
    break;
  }
  case msg_type_t::REPLACED:
    break;
  case msg_type_t::CANCELED:
    break;
  default:
    spdlog::warn("Unknown message type {}", static_cast<char>(msg_header->msg_type));
    break;
  }
}

int main(int argc, char *argv[]) {
  args_t args = parse_args(argc, argv);

  spdlog::info("Parsing {}", args.file_name);
  CMappedFileReader reader{args.file_name};

  if (ouch::parser::parse(reader, handler)) {
    spdlog::info("Parsing failed");
    return 1;
  }

  spdlog::info("Done");
  return 0;
}
