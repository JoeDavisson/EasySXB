#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "serial.h"

struct termios oldtio;

int serial_open(const char *device)
{
  struct termios newtio;
  int fd;

  fd = open(device, O_RDWR | O_NOCTTY);
  if (fd == -1)
  {
    printf("Couldn't open serial device.\n");
    exit(1);
  }

  tcgetattr(fd, &oldtio);

  memset(&newtio, 0, sizeof(struct termios));
  newtio.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;
  newtio.c_lflag = 0;
  newtio.c_cc[VTIME] = 0;
  newtio.c_cc[VMIN] = 1;

  tcflush(fd, TCIFLUSH);
  tcsetattr(fd, TCSANOW, &newtio);

  return fd;
}

void serial_close(int fd)
{
  tcsetattr(fd, TCSANOW, &oldtio);
  close(fd);
}

void serial_send(int fd, uint8_t *packet, int length)
{
  int n,sent;

  sent = 0;

  while(length - sent != 0)
  {
    n = write(fd, packet + sent, length - sent);

    if (n < 0)
    {
      printf("Waiting\n");
      sleep(1);
      continue;
    }

    sent += n;

    //printf("Sent %d, bytes left=%d\n", n, length - sent);
  }
}

int serial_read(int fd)
{
  uint8_t buffer[1];
  int n;

  buffer[0] = 0;

  while(1)
  {
    n = read(fd, buffer, 1);

//printf("n=%d buffer=%02x   fd=%d\n", n, buffer[0], fd);
    if (n < 0)
    {
      perror("serial_read");
      return -1;
    }

    if (n == 0) { continue; }

    //printf(">> %d %d %c\n", buffer[0], buffer[0] & 0x7f, buffer[0] & 0x7f);
    //fflush(stdout);

    return buffer[0];
  }
}


