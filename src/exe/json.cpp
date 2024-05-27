#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>

import ouch;
import ouch.parser;

import reader;

using json = nlohmann::json;

using namespace ouch;
json to_json(msg_header_t &m) {
  return json{{"message_length", m.message_length},
              {"packet_type", to_string(m.packet_type)},
              {"msg_type", to_string(m.msg_type)},
              {"timestamp", m.timestamp}};
}

json to_json(system_event_message_t &m) {
  json j = to_json(m.header);
  j["event_enum"] = to_string(m.event_enum);
  return j;
}

json to_json(accepted_message_t &m) {
  return json{
      {"header", to_json(m.header)},
      {"order_token", to_string(m.order_token)},
      {"side", to_string(m.side)},
      {"shares", m.shares},
      {"symbol", to_string(m.symbol)},
      {"price", m.price},
      {"time_in_force", m.time_in_force},
      {"firm", to_string(m.firm)},
      {"display", to_string(m.display)},
      {"order_reference_number", m.order_reference_number},
      {"order_capacity", m.order_capacity},
      {"intermarket_sweep", to_string(m.intermarket_sweep)},
      {"minimum_quantity", m.minimum_quantity},
      {"cross_type", to_string(m.cross_type)},
      {"order_state", to_string(m.order_state)},
  };
}

json to_json(replaced_message_t &m) {
  return json{
      {"header", to_json(m.header)},
      {"order_token", to_string(m.order_token)},
      {"side", to_string(m.side)},
      {"shares", m.shares},
      {"symbol", to_string(m.symbol)},
      {"price", m.price},
      {"time_in_force", m.time_in_force},
      {"firm", to_string(m.firm)},
      {"display", to_string(m.display)},
      {"order_reference_number", m.order_reference_number},
      {"order_capacity", m.order_capacity},
      {"intermarket_sweep", m.intermarket_sweep},
      {"minimum_quantity", m.minimum_quantity},
      {"cross_type", to_string(m.cross_type)},
      {"order_state", to_string(m.order_state)},
      {"previous_order_token", to_string(m.previous_order_token)},
  };
}

json to_json(executed_message_t &m) {
  return json{{"header", to_json(m.header)},
              {"order_token", to_string(m.order_token)},
              {"executed_shares", m.executed_shares},
              {"executed_price", m.executed_price},
              {"liquidity_flag", to_string(m.liquidity_flag)},
              {"match_number", m.match_number}};
}

json to_json(canceled_message_t &m) {
  return json{{"header", to_json(m.header)},
              {"order_token", to_string(m.order_token)},
              {"decrement_shares", m.decrement_shares},
              {"reason", to_string(m.reason)}
  };
}

void handler(const unsigned int, ouch::msg_header_t *msg_header) {

  using ouch::msg_type_t;
  switch (msg_header->msg_type) {
  case msg_type_t::SYSTEM_EVENT: {
    auto *system_event =
        std::bit_cast<ouch::system_event_message_t *>(msg_header);
    hn_swap(*system_event);
    std::cout << to_json(*system_event).dump() << std::endl;
    break;
  }
  case msg_type_t::ACCEPTED: {
    auto *accepted = std::bit_cast<ouch::accepted_message_t *>(msg_header);
    hn_swap(*accepted);
    std::cout << to_json(*accepted).dump() << std::endl;
    break;
  }
  case msg_type_t::EXECUTED: {
    auto *executed = std::bit_cast<ouch::executed_message_t *>(msg_header);
    hn_swap(*executed);
    std::cout << to_json(*executed).dump() << std::endl;
    break;
  }
  case msg_type_t::REPLACED: {
    auto *replaced = std::bit_cast<ouch::replaced_message_t *>(msg_header);
    hn_swap(*replaced);
    std::cout << to_json(*replaced).dump() << std::endl;
    break;
  }
  case msg_type_t::CANCELED: {
    auto *cancelled = std::bit_cast<ouch::canceled_message_t *>(msg_header);
    hn_swap(*cancelled);
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
