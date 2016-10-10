#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "serial.h"

int main(int argc, char *argv[])
{
  FILE *out;
  int count;
  int fd;
  int n;

  if (argc != 2)
  {
    printf("Usage: %s <filename>\n", argv[0]);
    exit(1);
  }

  out = fopen(argv[1], "wb");

  if (out == NULL)
  {
    printf("Could not open file '%s'\n", argv[1]);
    exit(1);
  }

  fd = serial_open("/dev/ttyUSB1");

  count = 0;

  while(1)
  {
    n = serial_read(fd);

    if (n < 0) { break; }

    putc(n, out);
    fflush(out);

    printf("%c", n);
    fflush(stdout);

    count++;
    //if ((count % 100) == 0) { printf("bytes transferred: %d\n", count); }
  }

  serial_close(fd);
  fclose(out);

  return 0;
}

