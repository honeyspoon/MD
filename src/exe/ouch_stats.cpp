#include <cstdint>

import ouch;
import ouch.parser;

import log;

import std;

import reader;

struct stats_t {
  int system_events = 0;
  int accepted = 0;
  int replaced = 0;
  int executed = 0;
  int executed_shares = 0;
  int cancelled = 0;
};

int system_events[ouch::MAX_STREAMS];
int accepted[ouch::MAX_STREAMS];
int replaced[ouch::MAX_STREAMS];
int executed[ouch::MAX_STREAMS];
int executed_shares[ouch::MAX_STREAMS];
int cancelled[ouch::MAX_STREAMS];

void print_stats(const stats_t &stats, const std::string name = "") {
  println("");
  println(name);
  println("- Accepted ", stats.accepted);
  println("- System Event ", stats.system_events);
  println("- Replaced ", stats.replaced);
  println("- Cancelled ", stats.cancelled);
  println("- Executed messages ", stats.executed, " | shares ",
          stats.executed_shares);
}

void aggregate_stats() {
  stats_t stats[ouch::MAX_STREAMS] = {};
  for (int i = 0; i < ouch::MAX_STREAMS; i++) {
    stats[i] = {system_events[i], accepted[i],        replaced[i],
                executed[i],      executed_shares[i], cancelled[i]};
  }

  // aggregate total stats
  stats_t total_stats = {-1};
  for (int i = 0; i < ouch::MAX_STREAMS; i++) {
    total_stats.system_events += system_events[i];
    total_stats.accepted += accepted[i];
    total_stats.replaced += replaced[i];
    total_stats.executed += executed[i];
    total_stats.executed_shares += executed_shares[i];
    total_stats.cancelled += cancelled[i];
  }

  // output
  for (int i = 0; i < ouch::MAX_STREAMS; i++) {
    if (stats[i].accepted)
      print_stats(stats[i], std::format("Stream ", i));
  }
  print_stats(total_stats, "Total");
}

using namespace ouch::parser;
void handleSystemEvent(stream_buffer_t &stream) { system_events[stream.id]++; }

void handleAccepted(stream_buffer_t &stream) { accepted[stream.id]++; }

void handleReplaced(stream_buffer_t &stream) { replaced[stream.id]++; }

void handleCanceled(stream_buffer_t &stream) { cancelled[stream.id]++; }

void handleExecuted(stream_buffer_t &stream) {
  executed[stream.id]++;
  auto *executed_msg =
      reinterpret_cast<ouch::executed_message_t *>(stream.buffer);
  executed_msg->executed_shares = ntohl(executed_msg->executed_shares);
  executed_shares[stream.id] += executed_msg->executed_shares;
}

void handler(stream_buffer_t &stream) {
  auto *msg_header = reinterpret_cast<ouch::msg_header_t *>(stream.buffer);
  using ouch::message_type_t;
  message_type_t msg_type =
      static_cast<message_type_t>(msg_header->message_type);

  switch (msg_type) {
  case message_type_t::SYSTEM_EVENT:
    handleSystemEvent(stream);
    break;
  case message_type_t::ACCEPTED:
    handleAccepted(stream);
    break;
  case message_type_t::EXECUTED:
    handleExecuted(stream);
    break;
  case message_type_t::REPLACED:
    handleReplaced(stream);
    break;
  case message_type_t::CANCELED:
    handleCanceled(stream);
    break;
  default:
    break;
    println("Unknown message type: ", static_cast<char>(msg_type));
  }
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

  CMappedFileReader reader{args.file_name};

  if (ouch::parser::parse(reader, handler)) {
    println("ERROR: analysis failed");
    return 1;
  }

  aggregate_stats();

  return 0;
}
