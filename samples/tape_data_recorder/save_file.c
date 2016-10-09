#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "serial.h"

int main(int argc, char *argv[])
{
  FILE *in;
  uint8_t buffer[1];
  int count;
  int fd;
  int ch, n;

  if (argc != 2)
  {
    printf("Usage: %s <filename>\n", argv[0]);
    exit(1);
  }

  in = fopen(argv[1], "rb");

  if (in == NULL)
  {
    printf("Could not open file '%s'\n", argv[1]);
    exit(1);
  }

  fd = serial_open("/dev/ttyUSB1");

  count = 0;

  while(1)
  {
    ch = getc(in);
    if (ch == EOF) { break; }

    buffer[0] = (uint32_t)ch;

    serial_send(fd, buffer, 1);
    n = serial_read(fd);

    if (n < 0) { break; }

    count++;
    if ((count % 100) == 0) { printf("bytes transferred: %d\n", count); }
  }

  serial_close(fd);
  fclose(in);

  return 0;
}

