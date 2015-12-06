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
#ifdef WIN32
  WSADATA wsa_data;
#endif

  char buf[1024];
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

/*
  void read(int fd)
  {
    struct timeval timeout;
    char c = '\0';
    int n;

    while((n = read(fd, &c, 1)) > 0)
    {
      putchar(c);
    }
  }
*/
}

void Terminal::connect()
{
  if(connected == true)
  {
    Dialog::message("Error", "Already connected to a server.");
    return;
  }

//  FILE *fp = fopen(argv[1], "r");

  fd = open("/dev/ttyUSB0", O_RDWR | O_NONBLOCK);
  if(fd == -1)
  {
    puts("Couldn't open serial port.");
    return;
  }

  memset(&term, 0, sizeof(struct termios));
  term.c_cflag = B9600 | CS8 | CLOCAL;
  term.c_iflag = IGNPAR;
  term.c_oflag = 0;
  term.c_lflag = 0;
  term.c_cc[VTIME] = 0;
  term.c_cc[VMIN] = 1;
  tcflush(fd, TCIFLUSH);
  tcsetattr(fd, TCSANOW, &term);
  fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

  connected = true;

  usleep(100000);
puts("connected");
}

void Terminal::disconnect()
{
  if(connected == true)
  {
    connected = false;
    Dialog::message("Disconnected", "Connection Closed");
  }
}

void Terminal::send(const char *message)
{
  if(connected == true)
  {
  }
}

void Terminal::receive(void *data)
{
  if(connected == true)
  {
    char c = '\0';
    int n;

    while((n = read(fd, &buf, sizeof(buf))) > 0)
    {
      for(int i = 0; i < sizeof(buf); i++)
      {
        if(buf[i] == 13)
          buf[i] = '\n';
      }

      char *current = strtok(buf, "\n");

      while(current != 0)
      {
//        int temp = write(STDOUT_FILENO, current, strlen(current));
//        temp = write(STDOUT_FILENO, "\n", 1);
        Gui::append(current);
        Gui::append("\n");
        current = strtok(0, "\n");

        if(current == 0)
          break;
      }
    }
  }

  Fl::repeat_timeout(.5, Terminal::receive, data);
}

