#include "outch.h"

#include "log.cpp"
#include "stats.cpp"
#include <stdio.h>
#include <iostream>

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

        bool is_complete(stream_buffer_t &stream)
        {
            auto *msg_header = reinterpret_cast<msg_header_t *>(stream.buffer);

            if (msg_header->message_length > MAX_MSG_LENGTH)
            {
                // no need to reconvert to little endian if it's already in the buffer
                msg_header->message_length = ntohs(msg_header->message_length);
                msg_header->timestamp = ntohll(msg_header->timestamp);
            }

            // incomplete message
            if (stream.offset != msg_header->message_length + 2)
            {
                return false;
            }
            stream.offset = 0;
            return true;
        }

        void on_executed_msg(stream_buffer_t &stream)
        {
            auto *executed_msg = reinterpret_cast<executed_message_t *>(stream.buffer);
            executed_msg->executed_shares = ntohl(executed_msg->executed_shares);
            // TODO: convert other fields
            stats::executed_shares[stream.id] += executed_msg->executed_shares;
        }

        int read_packet_header(FILE *file, packet_header_t &header)
        {
            fread(&header, sizeof(packet_header_t), 1, file);

            // big to little endian conversion
            header.stream_id = ntohs(header.stream_id);
            header.packet_length = ntohl(header.packet_length);

            if (ferror(file))
            {
                print("File read error!");
                return 1;
            }

            if (feof(file))
            {
                print("Unexpected end of file!");
                return 2;
            }

            return 0;
        }

        int read_msg(FILE *file, stream_buffer_t &stream, const uint32_t packet_length)
        {
            if (fread(stream.buffer + stream.offset, packet_length, 1, file))
            {
                stream.offset += packet_length;
            }

            if (ferror(file))
            {
                print("File read error!");
                return 1;
            }

            if (feof(file))
            {
                print("Unexpected end of file!");
                return 1;
            }

            return 0;
        }

        int parse_msg(stream_buffer_t &stream)
        {
            auto *msg_header = reinterpret_cast<msg_header_t *>(stream.buffer);
            uint8_t stream_id = stream.id;
            switch (msg_header->message_type)
            {
            case SYSTEM_EVENT:
                stats::system_events[stream_id]++;
                // TODO: handle
                break;
            case ACCEPTED:
                stats::accepted[stream_id]++;
                // TODO: handle
                break;
            case REPLACED:
                stats::replaced[stream_id]++;
                // TODO: handle
                break;
            case EXECUTED:
            {
                stats::executed[stream_id]++;
                on_executed_msg(stream);
                break;
            }
            case CANCELED:
                stats::cancelled[stream_id]++;
                // TODO: handle
                break;
            default:
                print("ERROR: unkown message type!", char(msg_header->message_type));
                return 1;
            }
            return 0;
        }

        stream_buffer_t stream_buffers[MAX_STREAMS];

        int analyse(const std::string file_name)
        {
            for (int i = 0; i < MAX_STREAMS; i++)
            {
                stream_buffers[i].offset = 0;
                stream_buffers[i].id = i;
            }

            print("running analysis on file", file_name);

            FILE *file = fopen(file_name.c_str(), "rb");
            if (!file)
            {
                print("ERROR: opening file");
            }

            packet_header_t header;
            while (read_packet_header(file, header) == 0)
            {
                stream_buffer_t &stream = stream_buffers[header.stream_id];

                if (read_msg(file, stream, header.packet_length))
                {
                    print("ERROR: reading message");
                    return 1;
                }

                if (!is_complete(stream))
                    continue;

                if (parse_msg(stream))
                {
                    print("ERROR: parsing message");
                    return 2;
                }
            }
            fclose(file);
            return 0;
        }
    }
}