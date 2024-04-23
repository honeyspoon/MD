#include "outch.h"

#include <stdio.h>
#include <iostream>

#include "log.cpp"

#include "stats.cpp"
#include "parser.cpp"

#if defined(__linux__)
#include <endian.h>
// no need for macos
uint64_t ntohll(uint64_t n)
{
  return be64toh(n);
}
#endif
struct args_t
{
  std::string file_name;
};

args_t parse_args(int argc, char *argv[])
{
  args_t args;

  if (argc < 2)
  {
    std::cerr << "Error: No filename provided" << std::endl;
    exit(1);
  }

  args.file_name = argv[1];

  return args;
}

int main(int argc, char *argv[])
{
  args_t args = parse_args(argc, argv);

  if (outch::parser::analyse(args.file_name))
  {
    print("ERROR: analysis failed");
    return 1;
  }

  stats::aggregate_stats();

  print("end");
}
