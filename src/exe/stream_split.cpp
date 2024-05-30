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
  args_t args = parse_args(argc, argv);

  mlog::info("Parsing OUCH file: {}", args.file_name);
  CMappedFileReader reader{args.file_name};

  if (parser::parse(reader, handler)) {
    mlog::error("error parsing file");
    return 1;
  }

  return 0;
}
