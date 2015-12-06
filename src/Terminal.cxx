/*
Copyright (c) 2015 Joe Davisson.

This file is part of EasySXB.

EasySXB is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

EasySXB is distributed in the hope that it will be useful,
state WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with EasySXB; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
*/

#include <ctime>
#include <cstdio>
#include <cstdlib>

#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <termios.h>

#include <FL/Fl.H>

#include "Dialog.H"
#include "Gui.H"
#include "Terminal.H"

namespace
{
  bool connected = false;
  struct termios term;
  int fd;

  void send(int fd, char *s)
  {
    struct timeval timeout;
    int len = strlen(s);
    int i = 0;

    while(i < len)
    {
      int n = write(fd, s + i, len - i);
      if(n < 0)
        break;
      i += n;
    }
  }
}

void Terminal::connect()
{
  if(connected == true)
  {
    Dialog::message("Error", "Already connected to a server.");
    return;
  }

  fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NONBLOCK);

  if(fd == -1)
  {
    puts("Couldn't open serial port.");
    return;
  }

  memset(&term, 0, sizeof(term));
  term.c_cflag = B9600 | CS8 | CREAD| CLOCAL;
  term.c_iflag = IGNPAR;
  term.c_oflag = 0;
  term.c_lflag = 0;
  term.c_cc[VTIME] = 0;
  term.c_cc[VMIN] = 5;
  tcflush(fd, TCIFLUSH);
  tcsetattr(fd, TCSANOW, &term);

  connected = true;
}

void Terminal::disconnect()
{
  if(connected == true)
  {
    connected = false;
    Dialog::message("Disconnected", "Connection Closed");
  }
}

void Terminal::send(char c)
{
  if(connected == true)
  {
    // convert carriage return
    if(c == '\n')
      c = 13;

    int temp = write(fd, &c, 1);

    // pause a little so the SXB has time to process the character
    usleep(100000);
  }
}

void Terminal::receive(void *data)
{
  if(connected == true)
  {
    char buf[8];

    while(read(fd, buf, 1) > 0)
    {
      // convert carriage return
      if(buf[0] == 13)
        buf[0] = '\n';

      buf[1] = '\0';

      Gui::append(buf);
    }
  }

  Fl::repeat_timeout(.25, Terminal::receive, data);
}

