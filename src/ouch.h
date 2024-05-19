#ifndef ouch_H
#define ouch_H

#include <stddef.h>

constexpr int MAX_STREAMS = 8;

typedef struct __attribute__((packed))
{
  uint16_t stream_id;
  uint32_t packet_length;
} packet_header_t;

enum __attribute__((packed)) packet_type_t : uint8_t
{
  SEQUENCED = 'S',
};

enum __attribute__((packed)) message_type_t : uint8_t
{
  SYSTEM_EVENT = 'S',
  ACCEPTED = 'A',
  REPLACED = 'U',
  EXECUTED = 'E',
  CANCELED = 'C',
};

typedef struct __attribute__((packed))
{
  uint16_t message_length;
  packet_type_t packet_type;
  message_type_t message_type;
  uint64_t timestamp;
} msg_header_t;

constexpr uint32_t MAX_MSG_LENGTH = 80;
typedef struct __attribute__((packed))
{
  msg_header_t header;
  char order_token[14];
  uint32_t executed_shares;
  uint32_t executed_price;
  char liquidity_flag;
  uint64_t match_number;
} executed_message_t;

#endif