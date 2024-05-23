#include <cstdint>

import ouch;
import ouch.parser;

import log;

import std;

import reader;
import writer;

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

std::map<uint8_t, std::unique_ptr<FileWriter>> writers;
void handler(uint8_t stream_id, const ouch::msg_header_t *msg_header) {
  auto it = writers.find(stream_id);
  if (it == writers.end()) {
    std::string file_name = std::format("out_{}.bin", stream_id);
    auto result =
        writers.emplace(stream_id, std::make_unique<FileWriter>(file_name));
    println("Created writer for stream id ", std::to_string(stream_id),
            " in file ", file_name);
    it = result.first;
  }

  ouch::packet_header_t packet_header{
      .stream_id = htons(stream_id),
      .packet_length =
          htonl(sizeof(uint16_t) + ntohs(msg_header->message_length))};

  it->second->write(reinterpret_cast<const char *>(&packet_header),
                    sizeof(packet_header));
  it->second->write(reinterpret_cast<const char *>(msg_header),
                    ntohs(msg_header->message_length) + sizeof(uint16_t));
}

int main(int argc, char *argv[]) {
  args_t args = parse_args(argc, argv);

  FileReader reader{args.file_name};

  if (ouch::parser::parse(reader, handler)) {
    println("ERROR: analysis failed");
    return 1;
  }

  return 0;
}
