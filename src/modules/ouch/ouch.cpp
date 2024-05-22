module;
#include <cstdint>
#include <stddef.h>

export module ouch;

export namespace ouch {

constexpr uint16_t MAX_STREAMS = 8;
constexpr uint16_t MAX_MSG_LENGTH = 80;

enum class __attribute__((packed)) packet_type_t : uint8_t {
  SEQUENCED = 'S',
};

enum class __attribute__((packed)) message_type_t : uint8_t {
  SYSTEM_EVENT = 'S',
  ACCEPTED = 'A',
  REPLACED = 'U',
  EXECUTED = 'E',
  CANCELED = 'C',
};

struct packet_header_t {
  uint16_t stream_id;
  uint32_t packet_length;
} __attribute__((packed));

struct msg_header_t {
  uint16_t message_length;
  packet_type_t packet_type;
  message_type_t message_type;
  uint64_t timestamp;
} __attribute__((packed));

struct executed_message_t {
  msg_header_t header;
  char order_token[14];
  uint32_t executed_shares;
  uint32_t executed_price;
  char liquidity_flag;
  uint64_t match_number;
} __attribute__((packed));

} // namespace ouch
