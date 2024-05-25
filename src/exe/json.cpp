#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

#include <nlohmann/json.hpp>

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

import ouch;
import ouch.parser;

import reader;

using json = nlohmann::json;

using namespace ouch;
json to_json(const msg_header_t &m) {
  return json{{"message_length", m.message_length},
              {"packet_type", std::string{static_cast<char>(m.packet_type)}},
              {"msg_type", std::string{static_cast<char>(m.msg_type)}},
              {"timestamp", m.timestamp}};
}

json to_json(const system_event_message_t &m) {
  json j = to_json(m.header);
  j["event_enum"] = std::string{static_cast<char>(m.event_enum)};
  return j;
}

json to_json(const accepted_message_t &m) {
  return json{
      {"header", to_json(m.header)},
      {"order_token",
       std::string(reinterpret_cast<const char *>(m.order_token), 14)},
      {"side", std::string{static_cast<char>(m.side)}},
      {"shares", m.shares},
      {"symbol", std::string(reinterpret_cast<const char *>(m.symbol), 8)},
      {"price", m.price},
      {"time_in_force", m.time_in_force},
      {"firm", std::string(reinterpret_cast<const char *>(m.firm), 4)},
      {"display", std::string{static_cast<const char>(m.display)}},
      {"order_reference_number", m.order_reference_number},
      {"order_capacity", m.order_capacity},
      {"intermarket_sweep", m.intermarket_sweep},
      {"minimum_quantity", m.minimum_quantity},
      {"cross_type", m.cross_type},
      {"order_state", m.order_state}};
}

json to_json(const replaced_message_t &m) {
  return json{
      {"header", to_json(m.header)},
      {"order_token",
       std::string(reinterpret_cast<const char *>(m.order_token), 14)},
      {"side", m.side},
      {"shares", m.shares},
      {"symbol", std::string(reinterpret_cast<const char *>(m.symbol), 8)},
      {"price", m.price},
      {"time_in_force", m.time_in_force},
      {"firm", std::string(reinterpret_cast<const char *>(m.firm), 4)},
      {"display", m.display},
      {"order_reference_number", m.order_reference_number},
      {"order_capacity", m.order_capacity},
      {"intermarket_sweep", m.intermarket_sweep},
      {"minimum_quantity", m.minimum_quantity},
      {"cross_type", m.cross_type},
      {"order_state", m.order_state},
      {"previous_order_token",
       std::string(reinterpret_cast<const char *>(m.previous_order_token),
                   14)}};
}

json to_json(const executed_message_t &m) {
  return json{{"header", to_json(m.header)},
              {"order_token",
               std::string(reinterpret_cast<const char *>(m.order_token), 14)},
              {"executed_shares", m.executed_shares},
              {"executed_price", m.executed_price},
              {"liquidity_flag", m.liquidity_flag},
              {"match_number", m.match_number}};
}

json to_json(const canceled_message_t &m) {
  return json{{"header", to_json(m.header)},
              {"order_token",
               std::string(reinterpret_cast<const char *>(m.order_token), 14)},
              {"decrement_shares", m.decrement_shares},
              {"reason", m.reason}};
}

void handler(const unsigned int, ouch::msg_header_t *msg_header) {

  using ouch::msg_type_t;
  switch (msg_header->msg_type) {
  case msg_type_t::SYSTEM_EVENT: {
    const auto *system_event =
        reinterpret_cast<const ouch::system_event_message_t *>(msg_header);
    std::cout << to_json(*system_event).dump() << std::endl;
    break;
  }
  case msg_type_t::ACCEPTED: {
    const auto *accepted =
        reinterpret_cast<const ouch::accepted_message_t *>(msg_header);
    std::cout << to_json(*accepted).dump() << std::endl;
    break;
  }
  case msg_type_t::EXECUTED: {
    const auto *executed =
        reinterpret_cast<const ouch::executed_message_t *>(msg_header);
    std::cout << to_json(*executed).dump() << std::endl;
    break;
  }
  case msg_type_t::REPLACED: {
    const auto *replaced =
        reinterpret_cast<const ouch::replaced_message_t *>(msg_header);
    std::cout << to_json(*replaced).dump() << std::endl;
    break;
  }
  case msg_type_t::CANCELED: {
    const auto *cancelled =
        reinterpret_cast<const ouch::canceled_message_t *>(msg_header);
    std::cout << to_json(*cancelled).dump() << std::endl;
    break;
  }
  default:
    spdlog::warn("Unknown message type {}",
                 static_cast<char>(msg_header->msg_type));
    break;
  }
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

int main(int argc, char *argv[]) {
  const auto stderr_logger = spdlog::stderr_color_mt("stderr");
  spdlog::set_default_logger(stderr_logger);

  const args_t args = parse_args(argc, argv);

  spdlog::info("Parsing {}", args.file_name);
  CMappedFileReader reader{args.file_name};

  if (ouch::parser::parse(reader, handler)) {
    spdlog::info("Parsing failed");
    return 1;
  }

  spdlog::info("Done");
  return 0;
}
