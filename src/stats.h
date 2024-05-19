
#pragma once

#include "parser.h"

namespace ouch {
namespace stats {

struct stats_t {
  std::string name;
  int system_events = 0;
  int accepted = 0;
  int replaced = 0;
  int executed = 0;
  int executed_shares = 0;
  int cancelled = 0;
};

void reset_stats();
void handler(parser::stream_buffer_t &stream);
void aggregate_stats();

} // namespace stats
} // namespace ouch
