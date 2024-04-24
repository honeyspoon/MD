#include "log.h"

#include "parser.h"

namespace outch
{
    namespace parser
    {

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
                print("End of file!");
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
                print("end of file!");
                return 1;
            }

            return 0;
        }

        stream_buffer_t stream_buffers[MAX_STREAMS];

        int analyse(const std::string file_name, MessageHandler handler)
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

                auto *msg_header = reinterpret_cast<msg_header_t *>(stream.buffer);
                uint8_t stream_id = stream.id;
                handler(stream);
            }
            fclose(file);
            return 0;
        }
    }
}