#include <cxxopts.hpp>

import std;

import mlog;
import ouch;
import ouch.parser;

import reader;
import writer;

using namespace ouch;
std::map<stream_id_t, std::unique_ptr<FileWriter>> writers;

void handler(stream_id_t stream_id, const msg_header_t *msg_header) {
  auto it = writers.find(stream_id);
  if (it == writers.end()) {
    std::string file_name = std::format("out_{}.bin", stream_id);
    auto result =
        writers.emplace(stream_id, std::make_unique<FileWriter>(file_name));
    mlog::info("Created writer for stream id {} in file {} ",
               std::to_string(stream_id), file_name);
    it = result.first;
  }

  auto msg_length =
      static_cast<packet_length_t>(hn_swap_copy(msg_header->message_length));
  auto packet_length =
      static_cast<packet_length_t>(sizeof(stream_id_t) + msg_length);
  packet_header_t packet_header{.stream_id = stream_id,
                                .packet_length = packet_length};
  hn_swap(packet_header.stream_id);
  hn_swap(packet_header.packet_length);

  it->second->write(std::bit_cast<const char *>(&packet_header),
                    sizeof(packet_header));
  it->second->write(std::bit_cast<const char *>(msg_header),
                    msg_length + sizeof(stream_id_t));
}

int main(int argc, char *argv[]) {
  cxxopts::Options options(
      "stream split",
      "split a file containing ouch streams into individual files per stream");

  options.add_options()("i,input_file", "Input file name",
                        cxxopts::value<std::string>())(
      "o,output_file", "Ouput file name", cxxopts::value<std::string>())(
      "s,symbol", "Symbol", cxxopts::value<std::string>());

  auto result = options.parse(argc, argv);

  auto input_file_name = result["input_file"].as<std::string>();
  mlog::info("Parsing OUCH file: {}", input_file_name);
  CMappedFileReader reader{input_file_name};

  if (parser::parse(reader, handler)) {
    mlog::error("error parsing file");
    return 1;
  }

  return 0;
}
