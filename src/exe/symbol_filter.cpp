#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

#include <bit>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <map>

import ouch;
import ouch.parser;

import reader;
import writer;

using namespace ouch;
void write(FileWriter &writer, int16_t stream_id,
           msg_header_t *msg_header) {
  packet_header_t packet_header{
      .stream_id = htons(stream_id),
      .packet_length =
          htonl(sizeof(uint16_t) + ntohs(msg_header->message_length))};

  writer.write(std::bit_cast<const char *>(&packet_header),
               sizeof(packet_header));
  writer.write(std::bit_cast<const char *>(msg_header),
               ntohs(msg_header->message_length) + sizeof(uint16_t));
}

struct args_t {
  std::string file_name;
};

const args_t parse_args(const int argc, char *argv[]) {
  args_t args;

  if (argc < 2) {
    std::cerr << "Error: No filename provided" << std::endl;
    std::exit(1);
  }

  args.file_name = argv[1];

  return args;
}

std::string pad(const std::string& str, std::size_t length) {
    return str + std::string(length - str.size(), ' ');
}

int main(int argc, char *argv[]) {
  const auto stderr_logger = spdlog::stderr_color_mt("stderr");
  spdlog::set_default_logger(stderr_logger);

  const args_t args = parse_args(argc, argv);

  spdlog::info("Parsing {}", args.file_name);
  CMappedFileReader reader{args.file_name};

  std::string symbol = "SMDV";
  FileWriter writer{std::format("{}.bin", symbol)};
  symbol = pad(symbol, 8);

  std::map<order_token_t, std::string> orders;
  const auto handler = [&writer, &orders, exp_symbol=symbol](const unsigned int id,
                                         msg_header_t *msg_header) {
    bool keep = true;

    switch (msg_header->msg_type) {
    case msg_type_t::SYSTEM_EVENT: {
      auto *system_event =
          std::bit_cast<system_event_message_t *>(msg_header);
      break;
    }
    case msg_type_t::ACCEPTED: {
      auto *accepted = std::bit_cast<accepted_message_t *>(msg_header);
      auto symbol = to_string(accepted->symbol);
      keep = exp_symbol == symbol;
      orders.emplace(accepted->order_token, symbol);
      break;
    }
    case msg_type_t::EXECUTED: {
      auto *executed = std::bit_cast<executed_message_t *>(msg_header);
      auto symbol = orders.find(executed->order_token)->second;
      keep = symbol == exp_symbol;
      orders.erase(executed->order_token);
      break;
    }
    case msg_type_t::REPLACED: {
      auto *replaced = std::bit_cast<replaced_message_t *>(msg_header);
      auto symbol = to_string(replaced->symbol);
      auto token  = replaced->order_token;
      keep = exp_symbol == symbol;
      orders.insert_or_assign(token, symbol);
      break;
    }
    case msg_type_t::CANCELED: {
      auto *cancelled = std::bit_cast<canceled_message_t *>(msg_header);
      auto symbol = orders.find(cancelled->order_token)->second;
      keep = symbol == exp_symbol;
      orders.erase(cancelled->order_token);
      break;
    }
    default:
      spdlog::warn("Unknown message type {}",
                   static_cast<char>(msg_header->msg_type));
      break;
    }

    if (keep)
      write(writer, id, msg_header);
  };

  if (parser::parse(reader, handler)) {
    spdlog::info("Parsing failed");
    return 1;
  }

  spdlog::info("Done");
  return 0;
}
