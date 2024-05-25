
#include <cstdint>
#include <iostream>
#include <map>

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

import ouch;
import ouch.parser;

import reader;
import book;

struct CArrayComparator {
    bool operator()(const uint8_t* lhs, const uint8_t* rhs) const {
        return std::memcmp(lhs, rhs, 8) < 0;
    }
};

std::map<uint8_t*, book::OrderBook, CArrayComparator> books;

void handler(uint8_t, ouch::msg_header_t *msg_header) {
  using ouch::msg_type_t;
  switch (msg_header->msg_type) {
  case msg_type_t::SYSTEM_EVENT:
      {
        auto* system_event = reinterpret_cast<ouch::system_event_message_t*>(msg_header);
        hn_swap(*system_event);
        break;
      }
  case msg_type_t::ACCEPTED:
      {
        auto* accepted = reinterpret_cast<ouch::accepted_message_t*>(msg_header);
        hn_swap(*accepted);

        const uint64_t order_id = accepted->order_reference_number;
        const int price = accepted->price;
        const int shares = accepted->shares;
        const uint8_t side = accepted->side;

        auto& book = books[accepted->symbol];
        auto& level = (side == 'B') ? book.bids[price] : book.asks[price];

        level.price = price;
        level.volume += shares;

        book.orders[order_id] = book::Order{price, shares, side};
        break;
      }
  case msg_type_t::EXECUTED: 
      {
        auto* executed = reinterpret_cast<ouch::executed_message_t*>(msg_header);

        hn_swap(*executed);
        break;
      }
  case msg_type_t::REPLACED:
      {
        auto* replaced = reinterpret_cast<ouch::replaced_message_t*>(msg_header);
        hn_swap(*replaced);
        break;
      }
  case msg_type_t::CANCELED:
      {
        auto* cancelled = reinterpret_cast<ouch::canceled_message_t*>(msg_header);
        hn_swap(*cancelled);
        break;
      }
  default:
    spdlog::warn("Unknown message type {}", static_cast<char>(msg_header->msg_type));
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

  for(auto const& [symbol, book] : books) {
    // const auto s_symbol = std::string{8, symbol};
    std::cout << "Book: " << symbol << std::endl;
    print_book(book);
  }

  spdlog::info("done");

  return 0;
}
