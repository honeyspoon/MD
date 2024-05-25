module;
#include <algorithm>
#include <array>
#include <bit>
#include <climits>
#include <concepts>
#include <cstdint>
#include <stddef.h>

export module ouch;

export namespace ouch {

constexpr uint16_t MAX_STREAMS = 8;
constexpr uint16_t MAX_MSG_LENGTH = 80;

enum class __attribute__((packed)) packet_type_t : uint8_t {
  SEQUENCED = 'S',
};

enum class __attribute__((packed)) msg_type_t : uint8_t {
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
static_assert(sizeof(packet_header_t) == 6);

struct msg_header_t {
  uint16_t message_length;
  packet_type_t packet_type;
  msg_type_t msg_type;
  uint64_t timestamp;
} __attribute__((packed));
static_assert(sizeof(msg_header_t) == 12);

enum event_enum_t : uint8_t {
    SOD = 'S',
    EOD ='E'
};

struct system_event_message_t {
  msg_header_t header;
  event_enum_t event_enum;
} __attribute__((packed));
static_assert(sizeof(system_event_message_t) == 13,
              "Size of system_event_message_t is incorrect");

struct accepted_message_t {
  msg_header_t header;
  uint8_t order_token[14];
  uint8_t side;
  uint32_t shares;
  uint8_t symbol[8];
  uint32_t price;
  uint32_t time_in_force;
  uint8_t firm[4];
  uint8_t display;
  uint64_t order_reference_number;
  uint8_t order_capacity;
  uint8_t intermarket_sweep;
  uint32_t minimum_quantity;
  uint8_t cross_type;
  uint8_t order_state;
} __attribute__((packed));
static_assert(sizeof(accepted_message_t) == 68,
              "Size of accepted_message_t is incorrect");

struct replaced_message_t {
  msg_header_t header;
  uint8_t order_token[14];
  uint8_t side;
  uint32_t shares;
  uint8_t symbol[8];
  uint32_t price;
  uint32_t time_in_force;
  uint8_t firm[4];
  uint8_t display;
  uint64_t order_reference_number;
  uint8_t order_capacity;
  uint8_t intermarket_sweep;
  uint32_t minimum_quantity;
  uint8_t cross_type;
  uint8_t order_state;
  uint8_t previous_order_token[14];
} __attribute__((packed));
static_assert(sizeof(replaced_message_t) == 82,
              "Size of replaced_message_t is incorrect");

struct executed_message_t {
  msg_header_t header;
  uint8_t order_token[14];
  uint32_t executed_shares;
  uint32_t executed_price;
  uint8_t liquidity_flag;
  uint64_t match_number;
} __attribute__((packed));
static_assert(sizeof(executed_message_t) == 43);

struct canceled_message_t {
  msg_header_t header;
  uint8_t order_token[14];
  uint32_t decrement_shares;
  uint8_t reason;
} __attribute__((packed));
static_assert(sizeof(canceled_message_t) == 31,
              "Size of canceled_message_t is incorrect");

template <std::integral T> void hn_swap(T &u) {
  if constexpr (sizeof(T) == 2) {
    u = __builtin_bswap16(u);
  } else if constexpr (sizeof(T) == 4) {
    u = __builtin_bswap32(u);
  } else if constexpr (sizeof(T) == 8) {
    u = __builtin_bswap64(u);
  } else {
    static_assert(sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8,
                  "Unsupported type size for endian swap");
  }
}

void hn_swap(packet_header_t &header) {
  hn_swap(header.stream_id);
  hn_swap(header.packet_length);
}

void hn_swap(msg_header_t &header) {
  hn_swap(header.message_length);
  hn_swap(header.timestamp);
}

void hn_swap(system_event_message_t &msg) { hn_swap(msg.header); }

void hn_swap(accepted_message_t &msg) {
  hn_swap(msg.header);
  hn_swap(msg.shares);
  hn_swap(msg.price);
  hn_swap(msg.time_in_force);
  hn_swap(msg.order_reference_number);
  hn_swap(msg.minimum_quantity);
}

void hn_swap(replaced_message_t &msg) {
  hn_swap(msg.header);
  hn_swap(msg.shares);
  hn_swap(msg.price);
  hn_swap(msg.time_in_force);
  hn_swap(msg.order_reference_number);
  hn_swap(msg.minimum_quantity);
}

void hn_swap(executed_message_t &msg) {
  hn_swap(msg.header);
  hn_swap(msg.executed_shares);
  hn_swap(msg.executed_price);
  hn_swap(msg.match_number);
}

void hn_swap(canceled_message_t &msg) {
  hn_swap(msg.header);
  hn_swap(msg.decrement_shares);
}

} // namespace ouch
