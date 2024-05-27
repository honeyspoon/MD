module;
#include <algorithm>
#include <array>
#include <bit>
#include <climits>
#include <concepts>
#include <cstdint>
#include <stddef.h>
#include <string>

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

enum class buy_sell_indicator_t : char {
    BUY = 'B',
    SELL = 'S',
    SELL_SHORT = 'T',
    SELL_SHORT_EXEMPT = 'E'
};

enum class display_type_t : char {
    ATTRIBUTABLE_PRICE = 'A',
    ANONYMOUS_PRICE = 'Y',
    NON_DISPLAY = 'N',
    POST_ONLY = 'P',
    IMBALANCE_ONLY = 'I',
    MID_POINT_PEG = 'M',
    MID_POINT_PEG_POST_ONLY = 'W',
    POST_ONLY_AND_ATTRIBUTABLE = 'L',
    RETAIL_ORDER_TYPE_1 = 'O',
    RETAIL_ORDER_TYPE_2 = 'T',
    RETAIL_PRICE_IMPROVEMENT_ORDER = 'Q',
    MID_POINT_PEG_AND_MID_POINT_TRADE_NOW = 'm',
    NON_DISPLAY_AND_MID_POINT_TRADE_NOW = 'n',
    MELO_AND_CONTINUOUS_BOOK_MIDPOINT = 'B'
};

enum class capacity_t : char {
    AGENCY = 'A',
    PRINCIPAL = 'P',
    RISKLESS = 'R',
    OTHER = 'O'
};

enum class intermarket_sweep_eligibility_t : char {
    ELIGIBLE = 'Y',
    NOT_ELIGIBLE = 'N',
    TRADE_AT_INTERMARKET_SWEEP_ORDER = 'y'
};

enum class cross_type_t : char {
    NO_CROSS = 'N',
    OPENING_CROSS = 'O',
    CLOSING_CROSS = 'C',
    HALT_IPO_CROSS = 'H',
    SUPPLEMENTAL_ORDER = 'S',
    EXTENDED_LIFE = 'E',
    EXTENDED_TRADING_CLOSE = 'A'
};

enum class customer_type_t : char {
    RETAIL_DESIGNATED_ORDER = 'R',
    NOT_RETAIL_DESIGNATED_ORDER = 'N',
    NONE = ' '
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

enum event_enum_t : uint8_t { SOD = 'S', EOD = 'E' };

struct system_event_message_t {
  msg_header_t header;
  event_enum_t event_enum;
} __attribute__((packed));
static_assert(sizeof(system_event_message_t) == 13,
              "Size of system_event_message_t is incorrect");


enum order_state_t : uint8_t { LIVE = 'L', DONE = 'D' };

typedef std::array<uint8_t, 8> symbol_t;
typedef std::array<uint8_t, 14> order_token_t;
typedef std::array<uint8_t, 4> firm_t;

template <std::size_t N>
std::string to_string(std::array<uint8_t, N> & e) {
  return std::string(reinterpret_cast<const char*>(e.data()), N);
}

template<typename EnumType, typename = std::enable_if_t<std::is_enum_v<EnumType>>>
std::string to_string(EnumType e) {
    return std::string(1, static_cast<char>(e));
}

struct accepted_message_t {
  msg_header_t header;
  order_token_t order_token;
  buy_sell_indicator_t side;
  uint32_t shares;
  symbol_t symbol;
  uint32_t price;
  uint32_t time_in_force;
  firm_t firm;
  display_type_t display;
  uint64_t order_reference_number;
  uint8_t order_capacity;
  intermarket_sweep_eligibility_t intermarket_sweep;
  uint32_t minimum_quantity;
  cross_type_t cross_type;
  order_state_t order_state;
} __attribute__((packed));
static_assert(sizeof(accepted_message_t) == 68,
              "Size of accepted_message_t is incorrect");

struct replaced_message_t {
  msg_header_t header;
  order_token_t order_token;
  buy_sell_indicator_t side;
  uint32_t shares;
  symbol_t symbol;
  uint32_t price;
  uint32_t time_in_force;
  firm_t firm;
  display_type_t display;
  uint64_t order_reference_number;
  uint8_t order_capacity;
  intermarket_sweep_eligibility_t intermarket_sweep;
  uint32_t minimum_quantity;
  cross_type_t cross_type;
  order_state_t order_state;
  order_token_t previous_order_token;
} __attribute__((packed));
static_assert(sizeof(replaced_message_t) == 82,
              "Size of replaced_message_t is incorrect");


enum class liquidity_flag_t : char {
    ADDED = 'A',
    REMOVED = 'R',
    OPENING_CROSS = 'O',
    OPENING_CROSS_IMBALANCE_ONLY = 'M',
    CLOSING_CROSS = 'C',
    CLOSING_CROSS_IMBALANCE_ONLY = 'L',
    HALT_IPO_CROSS = 'H',
    HALT_CROSS = 'K',
    NON_DISPLAYED_ADDING_LIQUIDITY = 'J',
    PASSIVE_MIDPOINT_EXECUTION = 'N',
    ADDED_POST_ONLY = 'W',
    REMOVED_LIQUIDITY_AT_MIDPOINT = 'm',
    ADDED_LIQUIDITY_VIA_MIDPOINT_ORDER = 'k',
    SUPPLEMENTAL_ORDER_EXECUTION = '0',
    DISPLAYED_LIQUIDITY_ADDING_ORDER_IMPROVES_NBBO = '7',
    DISPLAYED_LIQUIDITY_ADDING_ORDER_SETS_QBBO_JOINING_NBBO = '8',
    RETAIL_DESIGNATED_EXECUTION_REMOVED_LIQUIDITY = 'd',
    RETAIL_DESIGNATED_EXECUTION_ADDED_DISPLAYED_LIQUIDITY = 'e',
    RETAIL_DESIGNATED_EXECUTION_ADDED_NON_DISPLAYED_LIQUIDITY = 'f',
    RPI_ORDER_PROVIDES_LIQUIDITY = 'j',
    RETAIL_ORDER_REMOVES_RPI_LIQUIDITY = 'r',
    RETAIL_ORDER_REMOVES_PRICE_IMPROVING_NON_DISPLAYED_LIQUIDITY_OTHER_THAN_RPI_LIQUIDITY = 't',
    ADDED_DISPLAYED_LIQUIDITY_IN_GROUP_A_SYMBOL = '4',
    ADDED_NON_DISPLAYED_LIQUIDITY_IN_GROUP_A_SYMBOL = '5',
    REMOVED_LIQUIDITY_IN_GROUP_A_SYMBOL = '6',
    ADDED_NON_DISPLAYED_MIDPOINT_LIQUIDITY_IN_GROUP_A_SYMBOL = 'g',
    MIDPOINT_EXTENDED_LIFE_ORDER_EXECUTION = 'n',
    EXTENDED_TRADING_CLOSE_EXECUTION = 'i',
    ADDED_NON_DISPLAYED_LIQUIDITY_VIA_RESERVE_ORDER = 'u'
};

struct executed_message_t {
  msg_header_t header;
  order_token_t order_token;
  uint32_t executed_shares;
  uint32_t executed_price;
  liquidity_flag_t liquidity_flag;
  uint64_t match_number;
} __attribute__((packed));
static_assert(sizeof(executed_message_t) == 43);

enum class __attribute__((packed)) cancel_reason_t : uint8_t {
  USER_REQUESTED_CANCEL = 'U',
  IMMEDIATE_OR_CANCEL = 'I',
  TIMEOUT = 'T',
  SUPERVISORY = 'S',
  REGULATORY_RESTRICTION = 'D',
  SELF_MATCH_PREVENTION = 'Q',
  SYSTEM_CANCEL = 'Z',
  CROSS_CANCELED = 'C',
  MARKET_COLLARS = 'K',
  HALTED = 'H',
  OPEN_PROTECTION = 'X',
  CLOSED = 'E',
  POST_ONLY_CANCEL_NMS = 'F',
  POST_ONLY_CANCEL_CONTRA_SIDE = 'G'
};

struct canceled_message_t {
  msg_header_t header;
  order_token_t order_token;
  uint32_t decrement_shares;
  cancel_reason_t reason;
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
