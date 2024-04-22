#include <cinttypes>

/* typedef struct { */
typedef struct __attribute__((packed)) {
  uint16_t stream_id;
  uint32_t packet_length;
} packet_header_t;
