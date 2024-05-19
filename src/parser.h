
#pragma once
#include <iosfwd>
#include <functional>
#include <map>

#include "ouch.h"
#include "reader.h"
namespace ouch
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
        template <typename T>
        int parse(Reader<T> &reader, MessageHandler handler);
    }
}