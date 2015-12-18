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
#include <cctype>
#include <cstdio>
#include <cstdlib>

#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <fcntl.h>
#include <termios.h>

#include <FL/Fl.H>
#include <FL/Fl_Native_File_Chooser.H>

#include "Dialog.H"
#include "Gui.H"
#include "Terminal.H"

namespace
{
  bool connected = false;
  struct termios term;
  int fd;
  fd_set fs;
  struct timeval tv;
  int flash;

  // store previous directory paths
  char load_dir[256];

  // extract directory from a path/filename string
  void getDirectory(char *dest, const char *src)
  {
    strcpy(dest, src);

    int len = strlen(dest);
    if(len < 2)
      return;

    for(int i = len - 1; i > 0; i--)
    {
      if(dest[i - 1] == '/')
      {
        dest[i] = '\0';
        break;
      }
    }
  }
}

void Terminal::connect(const char *device)
{
  fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);

  if(fd == -1)
  {
    Dialog::message("Error", "Could not open serial port.");
    return;
  }

  memset(&term, 0, sizeof(term));
  term.c_cflag = B9600 | CS8 | CREAD| CLOCAL;
  term.c_iflag = IGNPAR;
  term.c_oflag = 0;
  term.c_lflag = 0;
  term.c_cc[VTIME] = 2;
  term.c_cc[VMIN] = 0;
  tcsetattr(fd, TCSANOW, &term);

  tv.tv_sec = 0;
  tv.tv_usec = 100000;

  flash = 0;
  connected = true;

  Gui::append("\n(Connected to SXB at 9600 baud.)\n");

  sleep(1);
  updateRegs();
}

void Terminal::disconnect()
{
  if(connected == true)
  {
    close(fd);
    connected = false;
    Gui::append("\n(Connection Closed.)\n");
    Dialog::message("Disconnected", "Connection Closed.");
  }
}

bool Terminal::isConnected()
{
  return connected;
}

void Terminal::sendChar(char c)
{
  if(connected == true)
  {
    FD_ZERO(&fs);
    FD_SET(fd, &fs);
    select(fd + 1, 0, &fs, 0, &tv);

    if(FD_ISSET(fd, &fs))
    {
      // convert to uppercase so it looks nice then the SXB echos the character
      c = toupper(c);

      // convert carriage return
      if(c == '\n')
        c = 13;

      int temp = write(fd, &c, 1);
    }
  }
}

char Terminal::getChar()
{
  char c;
  int tries = 0;

  if(connected == true)
  {
    FD_ZERO(&fs);
    FD_SET(fd, &fs);
    select(fd + 1, &fs, 0, 0, &tv);

    if(FD_ISSET(fd, &fs))
    {
      while(1)
      {
        int temp = read(fd, &c, 1);

        if(temp <= 0)
        {
          usleep(1000);
          tries++;
          if(tries > 100)
            return -1;
        }
        else
        {
          return c;
        }
      }
    }
  }
}

void Terminal::sendString(const char *s)
{
  if(connected == true)
  {
    for(int i = 0; i < strlen(s); i++)
    {
      char c = toupper(s[i]);

      // convert carriage return
      if(c == '\n')
        c = 13;

      sendChar(c);
    }
  }
}

void Terminal::getResult(char *s)
{
  if(connected == true)
  {
    int j = 0;

    while(1)
    {
      char c = getChar();

      if(c >= '0' && c <= '9' || c >= 'A' && c <= 'Z' || c == ' ')
        s[j++] = c;
      else if(c == 13)
        break;
    }

    s[j++] = '\n';
    s[j++] = '\0';
  }
}

void Terminal::receive(void *data)
{
  if(connected == true)
  {
    char buf[8];
    int n;

    FD_ZERO(&fs);
    FD_SET(fd, &fs);
    select(fd + 1, &fs, 0, 0, &tv);

    if(FD_ISSET(fd, &fs))
    {
      while(1)
      {
        int n = read(fd, buf, 1);

        if(n <= 0)
          break;

        // convert carriage return
        if(buf[0] == 13)
          buf[0] = '\n';

        buf[1] = '\0';

        Gui::append(buf);
      }
    }
  }

  // cause cursor to flash
  flash++;

  if(flash > 64)
    flash = 0;

  Gui::flashCursor((((flash >> 2) & 1) == 1) ? true : false);

  Fl::repeat_timeout(.1, Terminal::receive, data);
}

void Terminal::changeReg(int reg, int num)
{
  if(connected == false)
    return;

  char s[256];

  if(Gui::getMode() == Gui::MODE_265)
  {
    switch(reg)
    {
      case REG_PC:
        sprintf(s, "|P%02X:%04X", num >> 16, num & 0xFFFF);
        sendString(s);
        break;
      case REG_A:
        sprintf(s, "|A%04X", num);
        sendString(s);
        break;
      case REG_X:
        sprintf(s, "|X%04X", num);
        sendString(s);
        break;
      case REG_Y:
        sprintf(s, "|Y%04X", num);
        sendString(s);
        break;
      case REG_SP:
        sprintf(s, "|S%04X", num);
        sendString(s);
        break;
      case REG_DP:
        sprintf(s, "|D%04X", num);
        sendString(s);
        break;
      case REG_SR:
        sprintf(s, "|F%02X", num);
        sendString(s);
        break;
      case REG_DB:
        sprintf(s, "|B%02X", num);
        sendString(s);
        break;
    }

    sendString("R");

    if(reg == REG_SR)
      Gui::setToggles(num);
  }
  else if(Gui::getMode() == Gui::MODE_134)
  {
    switch(reg)
    {
      case REG_PC:
        sprintf(s, "A%04X     ", num & 0xFFFF);
        sendString(s);
        break;
      case REG_SR:
        sprintf(s, "A %02X    ", num & 0xFF);
        sendString(s);
        break;
      case REG_A:
        sprintf(s, "A  %02X   ", num);
        sendString(s);
        break;
      case REG_X:
        sprintf(s, "A   %02X  ", num);
        sendString(s);
        break;
      case REG_Y:
        sprintf(s, "A    %02X ", num);
        sendString(s);
        break;
      case REG_SP:
        sprintf(s, "A     %02X", num);
        sendString(s);
        break;
    }

    sendString("R");

    if(reg == REG_SR)
      Gui::setToggles(num);
  }
}

void Terminal::updateRegs()
{
  if(connected == false)
    return;

  char s[256];

  if(Gui::getMode() == Gui::MODE_265)
  {
    sendString("| ");
    getResult(s);
    Gui::updateRegs(s);
  }
  else if(Gui::getMode() == Gui::MODE_134)
  {
    sendString("R");
    getResult(s);
    getResult(s);
    getResult(s);
    Gui::updateRegs(s);
  }
}

void Terminal::jml(int address)
{
  if(connected == false)
    return;

  char s[256];

  if(Gui::getMode() == Gui::MODE_265)
  {
    sprintf(s, "G%02X%04X", address >> 16, address & 0xFFFF);
    sendString(s);
  }
  else if(Gui::getMode() == Gui::MODE_134)
  {
    sprintf(s, "G%04X", address & 0xFFFF);
    sendString(s);
  }
}

void Terminal::jsl(int address)
{
  if(connected == false)
    return;

  char s[256];

  if(Gui::getMode() == Gui::MODE_265)
  {
    sprintf(s, "J%02X%04X", address >> 16, address & 0xFFFF);
    sendString(s);
  }
  else if(Gui::getMode() == Gui::MODE_134)
  {
    sprintf(s, "J%04X", address & 0xFFFF);
    sendString(s);
  }
}

void Terminal::upload()
{
  if(connected == false)
  {
    Dialog::message("Error", "Not Connected.");
    return;
  }

  Fl_Native_File_Chooser fc;
  fc.title("Upload");
  fc.filter("HEX File\t*.hex\n");
  fc.options(Fl_Native_File_Chooser::PREVIEW);
  fc.type(Fl_Native_File_Chooser::BROWSE_FILE);
  fc.directory(load_dir);

  switch(fc.show())
  {
    case -1:
    case 1:
      return;
    default:
      getDirectory(load_dir, fc.filename());
      break;
  }

  int segment = 0;
  int address = 0;
  int code = 0;
  int value = 0;
  int count = 0;
  int temp;
  int ret;
  int i;
  char s[256];

  FILE *fp = fopen(fc.filename(), "r");
  if(!fp)
    return;

  if(Gui::getMode() == Gui::MODE_265)
  {
    while(1)
    {
      temp = fgetc(fp);
      if(temp == EOF)
        break;

      // start of line
      if(temp == ':')
      {
        segment = 0;
        ret = fscanf(fp, "%02X", &count);

        // last line
        if(count == 0x00)
        {
          break;
        }
        else
        {
          ret = fscanf(fp, "%04X", &address);
          ret = fscanf(fp, "%02X", &code);

          // if segment exists
          if(code == 0x04)
          {
            ret = segment = address;
          }
          else
          {
            int checksum = 0;

            // address
            sprintf(s, "S2%02X%02X%02X%02X",
                    count + 4, segment, address >> 8, address & 0xFF);
            sendString(s);

            checksum += count + 4;
            checksum += address >> 8;
            checksum += address & 0xFF;

            // data
            for(i = 0; i < count; i++)
            {
              ret = fscanf(fp, "%02X", &value);
              sprintf(s, "%02X", value);
              sendString(s);
              checksum += value;
            }

            // checksum
            sprintf(s, "%02X\n", 0xFF - (checksum & 0xFF));
            sendString(s);
          }
        }

        // skip to next line
        while(1)
        {
          temp = fgetc(fp);
          if(temp == '\n')
            break;
        }
      }
    }

    sprintf(s, "S804000000FB\n");
    sendString(s);
  }
  else if(Gui::getMode() == Gui::MODE_134)
  {
    while(1)
    {
      temp = fgetc(fp);
      if(temp == EOF)
        break;

      // start of line
      if(temp == ':')
      {
        segment = 0;
        ret = fscanf(fp, "%02X", &count);

        // last line
        if(count == 0x00)
        {
          break;
        }
        else
        {
          ret = fscanf(fp, "%04X", &address);
          ret = fscanf(fp, "%02X", &code);

          // if segment exists
          if(code == 0x04)
          {
            ret = segment = address;
          }
          else
          {
            int checksum = 0;

            // address
            sprintf(s, "S1%02X%02X%02X",
                    count + 3, address >> 8, address & 0xFF);
            sendString(s);

            checksum += count + 3;
            checksum += address >> 8;
            checksum += address & 0xFF;

            // data
            for(i = 0; i < count; i++)
            {
              ret = fscanf(fp, "%02X", &value);
              sprintf(s, "%02X", value);
              sendString(s);
              checksum += value;
            }

            // checksum
            sprintf(s, "%02X\n", 0xFF - (checksum & 0xFF));
            sendString(s);
          }
        }

        // skip to next line
        while(1)
        {
          temp = fgetc(fp);
          if(temp == '\n')
            break;
        }
      }
    }

    sprintf(s, "\n");
    sendString(s);
  }

  fclose(fp);
  Gui::append("\n(Upload Complete.)\n");
}

