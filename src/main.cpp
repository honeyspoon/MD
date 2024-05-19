
#include <iostream>
#include <print>

#include "parser.h"
#include "reader.h"
#include "stats.h"

struct args_t {
  std::string file_name;
};

args_t parse_args(int argc, char *argv[]) {
  args_t args;

  if (argc < 2) {
    std::cerr << "Error: No filename provided" << std::endl;
    exit(1);
  }

  args.file_name = argv[1];

  return args;
}

void handler(ouch::parser::stream_buffer_t &) {}
int main(int argc, char *argv[]) {
  args_t args = parse_args(argc, argv);

  using namespace ouch;

  // auto reader = create_reader<CMappedFileReader>(args.file_name);
  auto reader = create_reader<FileReader>(args.file_name);
  // auto reader = create_reader<CFileReader>(args.file_name);

  if (parser::parse(*reader, handler)) {
    std::println("ERROR: analysis failed");
    return 1;
  }

  // stats::aggregate_stats();

  std::println("end");
  return 0;
}
