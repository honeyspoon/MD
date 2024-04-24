
#pragma once
#include <iosfwd>
#include <functional>
#include <map>
#include "outch.h"

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

        using MessageHandler = void (*)(stream_buffer_t &);

        bool is_complete(stream_buffer_t &);

        void on_executed_msg(stream_buffer_t &);
        int read_packet_header(FILE *, packet_header_t &);
        int read_msg(FILE *, stream_buffer_t &, const uint32_t);
        int parse_msg(stream_buffer_t &);
        int analyse(const std::string, MessageHandler);
    }
}