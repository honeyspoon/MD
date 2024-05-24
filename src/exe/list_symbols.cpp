#include <cstdint>
#include <set>
#include <iostream>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

import ouch;
import ouch.parser;

import reader;

std::set<std::string> symbols;

void handler(uint8_t, const ouch::msg_header_t *msg_header) {
  using ouch::msg_type_t;
  switch (msg_header->msg_type) {
  case msg_type_t::SYSTEM_EVENT:
    break;
  case msg_type_t::ACCEPTED: {
    auto *accepted =
        reinterpret_cast<const ouch::accepted_message_t *>(msg_header);
    symbols.emplace(reinterpret_cast<const char *>(accepted->symbol), 8);
    break;
  }
  case msg_type_t::EXECUTED:
    break;
  case msg_type_t::REPLACED:
    break;
  case msg_type_t::CANCELED:
    break;
  default:
    spdlog::warn("Unknown message type {}",
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
  auto stderr_logger = spdlog::stderr_color_mt("stderr_logger");
  spdlog::set_default_logger(stderr_logger);

  args_t args = parse_args(argc, argv);

  spdlog::info("Parsing OUCH file: {}", args.file_name);
  FileReader reader{args.file_name};

  if (ouch::parser::parse(reader, handler)) {
    spdlog::error("error parsing file");
    return 1;
  }

  for (auto symbol : symbols) {
    std::cout << symbol << std::endl;
  }

  spdlog::info("done");

  return 0;
}
