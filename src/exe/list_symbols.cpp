import std;

import mlog;
import args;

import ouch;
import ouch.parser;
import reader;

using namespace ouch;
std::set<symbol_t> symbols;

void handler(const stream_id_t, msg_header_t *msg_header) {
  switch (msg_header->msg_type) {
  case msg_type_t::SYSTEM_EVENT:
    break;
  case msg_type_t::ACCEPTED: {
    auto *accepted = std::bit_cast<accepted_message_t *>(msg_header);
    symbols.emplace(accepted->symbol);
    break;
  }
  case msg_type_t::EXECUTED:
    break;
  case msg_type_t::REPLACED:
    break;
  case msg_type_t::CANCELED:
    break;
  default:
    mlog::warn("Unknown message type {}", static_cast<char>(msg_header->msg_type));
    break;
  }
}

int main(int argc, char *argv[]) {
  cxxopts::Options options("list symbols", "list symbols in ouch file");
  options.add_options() //
      ("i,input_file", "Input file name",
       cxxopts::value<std::string>()->default_value("-")) //
      ;

  const auto result = options.parse(argc, argv);
  const auto input_file = result["input_file"].as<std::string>();
  std::variant<StreamReader, FileReader> reader = StreamReader{std::cin};
  if (input_file != "-") {
    if (!std::filesystem::exists(input_file)) {
      mlog::error("File {} does not exist", input_file);
      std::exit(1);
    }

    mlog::info("Parsing {}", input_file);
    reader = FileReader{std::string(input_file)};
  }

  bool error = std::visit([](auto &&r) -> bool { return parser::parse(r, handler); }, reader);

  if (error) {
    mlog::error("error parsing file");
    return 1;
  }

  for (auto symbol : symbols) {
    std::cout << to_string(symbol) << std::endl;
  }

  mlog::info("done");

  return 0;
}
