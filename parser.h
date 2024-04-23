
#pragma once
#include "outch.h"

std::string;
namespace stats
{
    extern int system_events[MAX_STREAMS];
    extern int accepted[MAX_STREAMS];
    extern int replaced[MAX_STREAMS];
    extern int executed[MAX_STREAMS];
    extern int executed_shares[MAX_STREAMS];
    extern int cancelled[MAX_STREAMS];
}

namespace outch
{
    namespace parser
    {
        constexpr int BUFF_LEN = 128;
        typedef struct
        {
            uint8_t id;
            uint16_t offset;
            uint8_t buffer[BUFF_LEN];
        } stream_buffer_t;

        bool is_complete(stream_buffer_t &stream);

        void on_executed_msg(stream_buffer_t &stream);
        int read_packet_header(FILE *file, packet_header_t &header);
        int read_msg(FILE *file, stream_buffer_t &stream, const uint32_t packet_length);
        int parse_msg(stream_buffer_t &stream);
        int analyse(const std::string file_name);
    }
}