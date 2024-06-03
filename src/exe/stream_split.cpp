#include <cxxopts.hpp>

import std;

import mlog;
import ouch;
import ouch.parser;

import reader;
import writer;

using namespace ouch;

int main(int argc, char *argv[]) {
  cxxopts::Options options("stream split",
                           "split a file containing ouch streams into individual files per stream");

  options.add_options()                                                                      //
      ("i,input_file", "Input file name", cxxopts::value<std::string>()->default_value("-")) //
      ("o,output_dir", "Ouput dir name", cxxopts::value<std::string>())                      //
      ;

  auto result = options.parse(argc, argv);

  auto input_file = result["input_file"].as<std::string>();
  auto output_dir = result["output_dir"].as<std::string>();

  mlog::info("Parsing OUCH file: {}", input_file);
  mlog::info("Writing to: {}", output_dir);

  if (!std::filesystem::is_directory(output_dir) || !std::filesystem::exists(output_dir)) {
    if (!std::filesystem::create_directory(output_dir)) {
      mlog::error("Could not create directory {}", output_dir);
      std::exit(1);
    }
  }

  std::variant<StreamReader, FileReader> reader = StreamReader{std::cin};
  if (input_file != "-") {
    mlog::info("Parsing {}", input_file);
    reader = FileReader{std::string(input_file)};
  }

  bool error = std::visit(
      [output_dir](auto &&r) -> bool {
        std::map<stream_id_t, std::unique_ptr<FileWriter>> writers;
        auto handler = [&writers, output_dir](stream_id_t stream_id,
                                              const msg_header_t *msg_header) {
          auto it = writers.find(stream_id);
          if (it == writers.end()) {
            std::string file_name = std::format("{}/{}.bin", output_dir, stream_id);
            auto result = writers.emplace(stream_id, std::make_unique<FileWriter>(file_name));
            mlog::info("Created writer for stream id {} in file {} ", std::to_string(stream_id),
                       file_name);
            it = result.first;
          }

          auto msg_length = static_cast<packet_length_t>(hn_swap_copy(msg_header->message_length));
          auto packet_length = static_cast<packet_length_t>(sizeof(stream_id_t) + msg_length);
          packet_header_t packet_header{.stream_id = stream_id, .packet_length = packet_length};
          hn_swap(packet_header.stream_id);
          hn_swap(packet_header.packet_length);

          it->second->write(std::bit_cast<const char *>(&packet_header), sizeof(packet_header));
          it->second->write(std::bit_cast<const char *>(msg_header),
                            msg_length + sizeof(stream_id_t));
        };
        return parser::parse(r, handler);
      },
      reader);

  if (error) {
    mlog::error("error parsing file");
    return 1;
  }

  return 0;
}
