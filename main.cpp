#include <cinttypes>
#include <stdio.h>
#include <unistd.h>

#include "outch.h"
#include <arpa/inet.h>

#include <iostream>

template <typename T, typename... Args>
void print(const T &value, const Args &...args)
{
  std::cout << "-> " << value << " ";
  ((std::cout << args << " "), ...);
  std::cout << std::endl;
}

int main()
{
  /* FILE *file = fopen("output.little_endian", "rb"); */
  FILE *file = fopen("OUCHLMM2.incoming.packets", "rb");

  if (file)
  {
    packet_header_t header;

    print("start");
    while (fread(&header, sizeof(header), 1, file) == 1)
    {
      header.stream_id = ntohs(header.stream_id);
      uint32_t packet_length_be = header.packet_length;
      uint32_t packet_length_le = (packet_length_be >> 24) |
                                  ((packet_length_be >> 8) & 0xFF00) |
                                  ((packet_length_be << 8) & 0xFF0000) |
                                  (packet_length_be << 24);
      header.packet_length = packet_length_le;
      print("id:", header.stream_id);
      print("length:", header.packet_length);

      fseek(file, sizeof(header) + header.packet_length, SEEK_CUR);
    }
    print("end");

    fclose(file);
  }

  return 0;
}
