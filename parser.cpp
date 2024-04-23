#include "outch.h"

#include "log.cpp"
#include "stats.cpp"
#include <stdio.h>
#include <iostream>

constexpr int BUFF_LEN = 128;
typedef struct
{
    uint8_t id;
    uint16_t offset;
    uint8_t buffer[BUFF_LEN];
} stream_buffer_t;

void on_executed_msg(const stream_buffer_t &stream)
{
    executed_message_t *executed_msg = (executed_message_t *)stream.buffer;
    executed_msg->executed_shares = ntohl(executed_msg->executed_shares);
    // TODO: convert other fields
    executed_shares[stream.id] += executed_msg->executed_shares;
}

int read_packet_header(FILE *file, packet_header_t &header)
{
    int res = fread(&header, sizeof(packet_header_t), 1, file);

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

    return res == 1 ? 0 : 1;
}

int read_msg(FILE *file, stream_buffer_t &stream, const uint32_t packet_length, bool &complete)
{
    msg_header_t *msg_header = (msg_header_t *)stream.buffer;
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

    msg_header = (msg_header_t *)stream.buffer;

    if (msg_header->message_length > MAX_MSG_LENGTH)
    {
        // no need to reconvert to little endian if it's already in the buffer
        msg_header->message_length = ntohs(msg_header->message_length);
        msg_header->timestamp = ntohll(msg_header->timestamp);
    }

    // incomplete message
    if (stream.offset != msg_header->message_length + 2)
    {
        {
            complete = false;
            return 0;
        };
    }

    // we have a complete message and we can reset
    stream.offset = 0;
    complete = true;

    return 0;
}

int parse_msg(const stream_buffer_t &stream)
{
    msg_header_t *msg_header = (msg_header_t *)stream.buffer;
    uint8_t stream_id = stream.id;
    switch (msg_header->message_type)
    {
    case SYSTEM_EVENT:
        system_events[stream_id]++;
        // TODO: handle
        break;
    case ACCEPTED:
        accepted[stream_id]++;
        // TODO: handle
        break;
    case REPLACED:
        replaced[stream_id]++;
        // TODO: handle
        break;
    case EXECUTED:
    {
        executed[stream_id]++;
        on_executed_msg(stream);
        break;
    }
    case CANCELED:
        cancelled[stream_id]++;
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

        bool complete;
        if (read_msg(file, stream, header.packet_length, complete))
        {
            print("ERROR: reading message");
            return 1;
        }
        if (!complete)
            continue;

        if (parse_msg(stream))
        {
            print("ERROR: parsing message");
            return 2;
        }
    }
    fclose(file);

    aggregate_stats();
    return 0;
}