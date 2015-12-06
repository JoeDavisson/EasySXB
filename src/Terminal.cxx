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
}

void Terminal::connect(const char *address, const int port,
                   const bool keep_alive_value)
{
  if(connected == true)
  {
    Dialog::message("Error", "Already connected to a server.");
    return;
  }
}

void Terminal::disconnect()
{
  if(connected == true)
  {
    connected = false;
    Dialog::message("Disconnected", "Connection Closed");
  }
}

void Terminal::write(const char *message)
{
  if(connected == true)
  {
  }
}

void Terminal::read(void *data)
{
  if(connected == true)
  {
  }

  Fl::repeat_timeout(.5, Terminal::read, data);
}

