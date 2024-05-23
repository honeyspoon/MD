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


void handler(ouch::parser::stream_buffer_t &stream) {
   auto it = writers.find(stream.id);
   if (it == writers.end()) {
       auto result = writers.emplace(stream.id, std::make_unique<FileWriter>("out_" + std::to_string(stream.id)));
       it = result.first;
   }
   auto *msg_header = reinterpret_cast<ouch::msg_header_t *>(stream.buffer);
   it->second->write(reinterpret_cast<const char*>(stream.buffer), msg_header->message_length);
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
