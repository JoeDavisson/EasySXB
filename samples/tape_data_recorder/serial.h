
#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

int serial_open(const char *device);
void serial_close(int fd);
void serial_send(int fd, uint8_t *packet, int length);
int serial_read(int fd);

#endif

