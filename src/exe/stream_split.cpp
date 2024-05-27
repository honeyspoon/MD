import ouch;
import ouch.parser;

import reader;
import writer;

#include <cstdint>
#include <iostream>
#include <map>
#include <memory>

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"


std::map<uint8_t, std::unique_ptr<FileWriter>> writers;
void handler(uint8_t stream_id, const ouch::msg_header_t *msg_header) {
  auto it = writers.find(stream_id);
  if (it == writers.end()) {
    std::string file_name = std::format("out_{}.bin", stream_id);
    auto result =
        writers.emplace(stream_id, std::make_unique<FileWriter>(file_name));
    spdlog::info("Created writer for stream id {} in file {} ",
                 std::to_string(stream_id), file_name);
    it = result.first;
  }

  ouch::packet_header_t packet_header{
      .stream_id = htons(stream_id),
      .packet_length =
          htonl(sizeof(uint16_t) + ntohs(msg_header->message_length))};

  it->second->write(std::bit_cast<const char *>(&packet_header),
                    sizeof(packet_header));
  it->second->write(std::bit_cast<const char *>(msg_header),
                    ntohs(msg_header->message_length) + sizeof(uint16_t));
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
  CMappedFileReader reader{args.file_name};

  if (ouch::parser::parse(reader, handler)) {
    spdlog::error("error parsing file");
    return 1;
  }

  return 0;
}
