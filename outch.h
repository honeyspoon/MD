#include <assert.h>
#include <stddef.h>

typedef struct __attribute__((packed)) {
  uint16_t stream_id;
  uint32_t packet_length;
} packet_header_t;
static_assert(sizeof(packet_header_t) == 6,
              "packet_header struct size mismatch");

enum __attribute__((packed)) packet_type_t : uint8_t {
  SEQUENCED = 'S',
};
static_assert(sizeof(packet_type_t) == 1, "packet type struct size mismatch");

enum __attribute__((packed)) message_type_t : uint8_t {
  SYSTEM_EVENT = 'S',
  ACCEPTED = 'A',
  REPLACED = 'U',
  EXECUTED = 'E',
  CANCELED = 'C',
};
static_assert(sizeof(message_type_t) == 1, "message type struct size mismatch");

typedef struct __attribute__((packed)) {
  uint16_t message_length;
  packet_type_t packet_type;
  message_type_t message_type;
  uint64_t timestamp;
} outch_header_t;
static_assert(sizeof(outch_header_t) == 12, "packet type struct size mismatch");

typedef struct __attribute__((packed)) {
  outch_header_t header;
  char order_token[14];
  uint32_t executed_shares;
  uint32_t executed_price;
  char liquidity_flag;
  uint64_t match_number;
} executed_message_t;

static_assert(sizeof(executed_message_t) == 43,
              "Executed Message struct size mismatch");
