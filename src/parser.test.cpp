
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "log.h"
#include "parser.h"

namespace outch
{
    namespace parser
    {
        TEST_CASE("SYSTEM_EVENT message increments system_events count")
        {
            stream_buffer_t stream;
            stream.id = 4;
            msg_header_t header = {SYSTEM_EVENT};
            std::memcpy(stream.buffer, &header, sizeof(header));

            int result = parse_msg(stream);

            REQUIRE(result == 0);
        }

        TEST_CASE("ACCEPTED message increments accepted count")
        {
            stream_buffer_t stream;
            stream.id = 2;
            msg_header_t header = {ACCEPTED};
            std::memcpy(stream.buffer, &header, sizeof(header));

            int result = parse_msg(stream);

            REQUIRE(result == 0);
        }

    }
}