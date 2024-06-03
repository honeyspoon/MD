#include <cxxopts.hpp>

import std;
import mlog;

import ouch;
import ouch.parser;

import reader;
import writer;

using namespace ouch;

void write(Writable auto &writer, stream_id_t stream_id, msg_header_t *msg_header) {
  auto msg_length = static_cast<packet_length_t>(hn_swap_copy(msg_header->message_length));
  auto packet_length = static_cast<packet_length_t>(sizeof(stream_id_t) + msg_length);
  packet_header_t packet_header{.stream_id = stream_id, .packet_length = packet_length};
  hn_swap(packet_header.stream_id);
  hn_swap(packet_header.packet_length);

  writer.write(std::bit_cast<const char *>(&packet_header), sizeof(packet_header));
  writer.write(std::bit_cast<const char *>(msg_header), msg_length + sizeof(stream_id_t));
}

std::string pad(const std::string &str, std::size_t length) {
  return str + std::string(length - str.size(), ' ');
}

bool maches_symbol(const std::string_view exp_symbol, const stream_id_t id,
                   msg_header_t *msg_header) {
  static std::map<order_token_t, std::string> orders;

  bool keep = true;
  switch (msg_header->msg_type) {
  case msg_type_t::SYSTEM_EVENT: {
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
    auto token = replaced->order_token;
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
    mlog::warn("Unknown message type {}", static_cast<char>(msg_header->msg_type));
    break;
  }

  return keep;
};

int main(int argc, char *argv[]) {
  cxxopts::Options options("MyProgram", "One line description of MyProgram");

  options.add_options()                                                   //
      ("i,input_file", "Input file name", cxxopts::value<std::string>())  //
      ("o,output_file", "Ouput file name", cxxopts::value<std::string>()) //
      ("s,symbol", "Symbol", cxxopts::value<std::string>())               //
      ;

  auto result = options.parse(argc, argv);

  auto input_file = result["input_file"].as<std::string>();
  auto output_file = result["output_file"].as<std::string>();
  auto symbol = result["symbol"].as<std::string>();

  mlog::info("Keeping symbol {}", symbol);

  std::variant<StreamReader, FileReader> reader = StreamReader{std::cin};
  if (input_file != "-") {
    mlog::info("Parsing {}", input_file);
    reader = FileReader{std::string(input_file)};
  }

  std::variant<StreamWriter, FileWriter> writer = StreamWriter{std::cout};
  if (output_file != "-") {
    mlog::info("Outputing to {}", output_file);
    writer = FileWriter{std::string(output_file)};
  }

  bool error = std::visit(
      [symbol](auto &r, auto &w) -> bool {
        std::string p_symbol = pad(symbol, 8);
        const auto handler = [p_symbol, &w](stream_id_t id, msg_header_t *msg_header) {
          if (maches_symbol(p_symbol, id, msg_header)) {
            write(w, id, msg_header);
          }
        };
        return parser::parse(r, handler);
      },
      reader, writer);

  if (error) {
    mlog::error("error parsing file");
    return 1;
  }

  mlog::info("Done");
  return 0;
}
