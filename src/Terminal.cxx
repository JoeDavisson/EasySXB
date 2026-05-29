/*
Copyright (c) 2023 Joe Davisson.

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

#ifndef WIN32
  #include <unistd.h>
  #include <string.h>
  #include <sys/socket.h>
  #include <sys/time.h>
  #include <fcntl.h>
  #include <termios.h>
#else
  #include <windows.h>
#endif

#include <FL/Fl.H>
#include <FL/Fl_Native_File_Chooser.H>

#include "Dialog.H"
#include "Gui.H"
#include "Terminal.H"

// for Visual Studio
#if defined(_MSC_VER)
#define strcasecmp _stricmp 
#endif

namespace
{
  bool connected = false;
  bool can_receive = false;
  int flash;
  char buf[4096];
  char input_buf[4096];
  int buf_pos = 0;

#ifdef WIN32
  HANDLE hserial;
  DCB dcb;
  COMMTIMEOUTS timeouts;
#else
  struct termios term;
  int fd;
  struct timeval tv;
#endif

  // store previous directory paths
  char load_dir[4096];

  // extract directory from a path/filename string
  void getDirectory(char *dest, const char *src)
  {
    snprintf(dest, sizeof(*dest), "%s", src);

    int len = strlen(dest);

    if (len < 2)
      return;

    for (int i = len - 1; i > 0; i--)
    {
      if (dest[i - 1] == '/')
      {
        dest[i] = '\0';
        break;
      }
    }
  }

  void fileError()
  {
    Dialog::message("Error", "Error reading file.\n");
  }

  void wait(const int ms)
  {
#ifdef WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
  }

  void clearBuf()
  {
    memset(buf, 0, sizeof(buf));
    buf_pos = 0;
  }
}

char Terminal::port_string[4096];

void Terminal::connect(int hardware_flow)
{
#ifdef WIN32
  // correct port name
  snprintf(buf, sizeof(buf), "\\\\.\\%s", port_string);

  hserial = CreateFile(port_string, GENERIC_READ | GENERIC_WRITE,
                       0, NULL, OPEN_EXISTING, 0, NULL);

  if (hserial == INVALID_HANDLE_VALUE)
  {
    Dialog::message("Error", "Could not open serial port. (CreateFile)");
    return;
  }

  memset(&dcb, 0, sizeof(dcb));
  BOOL ret = GetCommState(hserial, &dcb);

  if (ret == FALSE)
  {
    Dialog::message("Error", "Could not open serial port. (GetCommState)");
    return;
  }

  dcb.DCBlength = sizeof(dcb);
  dcb.BaudRate = CBR_9600;
  dcb.ByteSize = 8;
  dcb.StopBits = ONESTOPBIT;
  dcb.Parity = NOPARITY;
  dcb.fOutX = FALSE;
  dcb.fInX = FALSE;
  dcb.fOutxCtsFlow = TRUE;

  if (hardware_flow)
    dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
  else
    dcb.fRtsControl = RTS_CONTROL_DISABLE;

  dcb.fOutxDsrFlow = FALSE;
  dcb.fDtrControl = DTR_CONTROL_DISABLE;

  ret = SetCommState(hserial, &dcb);

  if (ret == FALSE)
  {
    CloseHandle(hserial);
    Dialog::message("Error", "Could not open serial port. (SetCommState)");
    return;
  }

  memset(&timeouts, 0, sizeof(timeouts));

  timeouts.ReadIntervalTimeout = MAXDWORD;
  timeouts.ReadTotalTimeoutConstant = 0;
  timeouts.ReadTotalTimeoutMultiplier = 0;
  timeouts.WriteTotalTimeoutConstant = 0;
  timeouts.WriteTotalTimeoutMultiplier = 0;

  ret = SetCommTimeouts(hserial, &timeouts);

  if (ret == FALSE)
  {
    CloseHandle(hserial);
    Dialog::message("Error", "Could not open serial port. (SetCommTimeouts)");
    return;
  }

  if (ret == FALSE)
  {
    CloseHandle(hserial);
    Dialog::message("Error", "Could not open serial port. (CloseHandle)");
    return;
  }
#else
  fd = open(port_string, O_RDWR | O_NOCTTY | O_NONBLOCK);

  if (fd == -1)
  {
    Dialog::message("Error", "Could not open serial port.");
    return;
  }

  memset(&term, 0, sizeof(term));

  if (hardware_flow)
    term.c_cflag = B9600 | CRTSCTS | CS8 | CREAD | CLOCAL;
  else
    term.c_cflag = B9600 | CS8 | CREAD | CLOCAL;

  term.c_iflag = IGNPAR;
  term.c_oflag = 0;
  term.c_lflag = 0;
  term.c_cc[VTIME] = 1;
  term.c_cc[VMIN] = 1;

  cfsetispeed(&term, B9600);
  cfsetospeed(&term, B9600);
  tcflush(fd, TCIFLUSH);

  int result = tcsetattr(fd, TCSANOW, &term);

  if (result == -1)
  {
    Dialog::message("Error", "tcsetattr() failed.");
    return;
  }

  tv.tv_sec = 0;
  tv.tv_usec = 100000;
#endif

  buf_pos = 0;
  flash = 0;
  connected = true;
  can_receive = true;

  Gui::append("\n>> Connected to SXB at 9600 baud.\n\n");
  Gui::append("\n");
}

void Terminal::disconnect()
{
  if (connected == true)
  {
#ifdef WIN32
    CloseHandle(hserial);
#else
    close(fd);
#endif
    connected = false;
    Gui::append("\n>> Connection Closed.\n");
    Dialog::message("Disconnected", "Connection Closed.");
  }
}

bool Terminal::isConnected()
{
  return connected;
}

void Terminal::sendString(const char *s)
{
  if (connected == true)
  {
    memset(input_buf, 0, sizeof(input_buf));
    snprintf(input_buf, sizeof(input_buf), "%s", s);

    for (unsigned int i = 0; i < strlen(input_buf); i++)
    {
      if (input_buf[i] == '\n')
        input_buf[i] = 13;
    }

#ifdef WIN32
    DWORD bytes;
    WriteFile(hserial, input_buf, strlen(input_buf), &bytes, NULL);
    FlushFileBuffers(hserial);
#else
    int bytes = write(fd, input_buf, strlen(input_buf));
    tcdrain(fd);
#endif
  }
}

// sends string one byte at a time
// (some ROM commands seem to expect this)
void Terminal::sendStringByChar(const char *s)
{
  if (connected == true)
  {
    memset(input_buf, 0, sizeof(input_buf));
    snprintf(input_buf, sizeof(input_buf), "%s", s);

    for (unsigned int i = 0; i < strlen(input_buf); i++)
    {
      if (input_buf[i] == '\n')
        input_buf[i] = 13;

#ifdef WIN32
      DWORD bytes;
      WriteFile(hserial, input_buf + i, 1, &bytes, NULL);
      FlushFileBuffers(hserial);
#else
      int bytes = write(fd, input_buf + i, 1);
      tcdrain(fd);
#endif
    }
  }

  wait(100);
}

void Terminal::getResult(char *s)
{
  if (connected == true)
  {
    wait(500);
    can_receive = false;

    clearBuf();
    getData();

    can_receive = true;
    Gui::append(buf);

    int j = 0;

    for (unsigned int i = 0; i < 255; i++)
    {
      char c = buf[i];

      if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || c == ' ')
        s[j++] = c;
    }

    s[j] = '\0';
    clearBuf();
  }
}

void Terminal::getData()
{
  if (connected == false)
    return;

#ifdef WIN32
  DWORD bytes;

  while (true)
  {
    BOOL temp = ReadFile(hserial, buf + buf_pos, 16, &bytes, NULL);

    if (temp == 0 || bytes == 0)
      break;

    buf_pos += bytes;

    if (buf_pos > 2048)
      break;
  }
#else
  int bytes;

  while (true)
  {
    bytes = read(fd, buf + buf_pos, 16);

    if (bytes <= 0)
      break;

    buf_pos += bytes;

    if (buf_pos > 2048)
      break;
  }
#endif

  for (unsigned int i = 0; i < sizeof(buf); i++)
  {
    if (buf[i] == 13)
      buf[i] = '\n';
  }
}

void Terminal::receive(void *data)
{
  if (can_receive == false)
  {
    Fl::repeat_timeout(.05, Terminal::receive, data);
    return;
  }

  getData();

  if (strchr(buf, '\n') != 0 || strchr(buf, '\0') != 0)
  {
    Gui::append(buf);
    clearBuf();
  }

  // cause cursor to flash
  flash++;

  if (flash > 63)
    flash = 0;

  Gui::flashCursor((((flash >> 2) & 1) == 1) ? true : false);

  Fl::repeat_timeout(.05, Terminal::receive, data);
}

void Terminal::changeReg(int reg, int num)
{
  if (connected == false)
    return;

  if (num < 0)
    num = 0;

  char s[256];

  if (Gui::getMode() == Gui::MODE_265)
  {
    switch (reg)
    {
      case REG_PC:
        snprintf(s, sizeof(s), "|P%02X:%04X", num >> 16, num & 0xFFFF);
        sendStringByChar(s);
        break;
      case REG_A:
        snprintf(s, sizeof(s), "|A%04X", num);
        sendStringByChar(s);
        break;
      case REG_X:
        snprintf(s, sizeof(s), "|X%04X", num);
        sendStringByChar(s);
        break;
      case REG_Y:
        snprintf(s, sizeof(s), "|Y%04X", num);
        sendStringByChar(s);
        break;
      case REG_SP:
        snprintf(s, sizeof(s), "|S%04X", num);
        sendStringByChar(s);
        break;
      case REG_DP:
        snprintf(s, sizeof(s), "|D%04X", num);
        sendStringByChar(s);
        break;
      case REG_SR:
        snprintf(s, sizeof(s), "|F%02X", num);
        sendStringByChar(s);
        break;
      case REG_DB:
        snprintf(s, sizeof(s), "|B%02X", num);
        sendStringByChar(s);
        break;
    }

    sendStringByChar("R");

    if (reg == REG_SR)
      Gui::setToggles(num);
  }
  else if (Gui::getMode() == Gui::MODE_134)
  {
    switch (reg)
    {
      case REG_PC:
        snprintf(s, sizeof(s), "A%04X     ", num & 0xFFFF);
        sendStringByChar(s);
        break;
      case REG_SR:
        snprintf(s, sizeof(s), "A %02X    ", num & 0xFF);
        sendStringByChar(s);
        break;
      case REG_A:
        snprintf(s, sizeof(s), "A  %02X   ", num);
        sendStringByChar(s);
        break;
      case REG_X:
        snprintf(s, sizeof(s), "A   %02X  ", num);
        sendStringByChar(s);
        break;
      case REG_Y:
        snprintf(s, sizeof(s), "A    %02X ", num);
        sendStringByChar(s);
        break;
      case REG_SP:
        snprintf(s, sizeof(s), "A     %02X", num);
        sendStringByChar(s);
        break;
    }

    sendString("R");

    if (reg == REG_SR)
      Gui::setToggles(num);
  }
}

void Terminal::updateRegs()
{
  if (connected == false)
    return;

  char s[256];
  memset(s, 0, sizeof(s));

  if (Gui::getMode() == Gui::MODE_265)
  {
    sendStringByChar("| ");
    getResult(s);
    Gui::updateRegs(s);
  }
  else if (Gui::getMode() == Gui::MODE_134)
  {
    sendStringByChar("R");
    getResult(s);
    Gui::updateRegs(s);
  }
}

void Terminal::jml(int address)
{
  if (connected == false)
    return;

  if (address < 0)
    address = 0;

  char s[256];

  if (Gui::getMode() == Gui::MODE_265)
  {
    sendStringByChar("G");
    snprintf(s, sizeof(s), "%02X%04X", address >> 16, address & 0xFFFF);
    sendStringByChar(s);
  }
  else if (Gui::getMode() == Gui::MODE_134)
  {
    sendStringByChar("G");
    snprintf(s, sizeof(s), "%04X", address & 0xFFFF);
    sendStringByChar(s);
  }
}

void Terminal::jsl(int address)
{
  if (connected == false)
    return;

  if (address < 0)
    address = 0;

  char s[256];

  if (Gui::getMode() == Gui::MODE_265)
  {
    sendString("J");
    snprintf(s, sizeof(s), "%02X%04X", address >> 16, address & 0xFFFF);
    sendStringByChar(s);
  }
  else if (Gui::getMode() == Gui::MODE_134)
  {
    sendString("J");
    snprintf(s, sizeof(s), "%04X", address & 0xFFFF);
    sendStringByChar(s);
  }
}

void Terminal::dump(int address)
{
  if (connected == false)
    return;

  if (address < 0)
    address = 0;

  char s[256];

  snprintf(s, sizeof(s), "\nMemory dump from %02X:%04X - %02X:%04X\n",
          address >> 16, address & 0xFFFF,
          (address + 0xff) >> 16, (address + 0xff) & 0xFFFF);
  Gui::append(s);

  if (Gui::getMode() == Gui::MODE_265)
  {
    sendStringByChar("D");
    snprintf(s, sizeof(s), "%02X%04X", address >> 16, address & 0xFFFF);
    sendStringByChar(s);
    snprintf(s, sizeof(s), "%02X%04X", (address + 0xff) >> 16, (address + 0xff) & 0xFFFF);
    sendStringByChar(s);
  }
  else if (Gui::getMode() == Gui::MODE_134)
  {
    sendStringByChar("D");
    snprintf(s, sizeof(s), "%04X%04X", address & 0xFFFF, (address + 0xff) & 0xffff);
    sendStringByChar(s);
  }

  sendStringByChar("\n");
}

void Terminal::upload()
{
  if (connected == false)
  {
    Dialog::message("Error", "Not Connected.");
    return;
  }

  Fl_Native_File_Chooser fc;
  fc.title("Upload Program");
  fc.filter("HEX File\t*.hex\nSREC File\t*.srec\n");
  fc.options(Fl_Native_File_Chooser::PREVIEW);
  fc.type(Fl_Native_File_Chooser::BROWSE_FILE);
  fc.directory(load_dir);

  switch (fc.show())
  {
    case -1:
    case 1:
      return;
    default:
      getDirectory(load_dir, fc.filename());
      break;
  }

  const char *ext = fl_filename_ext(fc.filename());

  if (strcasecmp(ext, ".hex") == 0)
  {
    Terminal::uploadHex(fc.filename());
  }
  else if (strcasecmp(ext, ".srec") == 0)
  {
    Terminal::uploadSrec(fc.filename());
  }
    else
  {
    Dialog::message("Upload Error",
                    "Only .hex and .srec file extentions are supported.");
  }
}

void Terminal::uploadHex(const char *filename)
{
  int segment = 0;
  int address = 0;
  int start_address = -1;
  int code = 0;
  int value = 0;
  int count = 0;
  int temp;
  int ret;
  char s[256];

  FILE *fp = fopen(filename, "r");

  if (fp == NULL)
  {
    Dialog::message("Error", "Could not open file.\n");
    return;
  }

  Gui::append("\n>> Uploading Program, ESC to cancel.\n");

  while (1)
  {
    temp = fgetc(fp);

    if (temp == EOF)
      break; 

    // start of line
    if (temp == ':')
    {
      segment = 0;
      ret = fscanf(fp, "%02X", &count);

      if (ret == -1 || ret == EOF)
      {
        fileError();
        break;
      }

      // last line
      if (count == 0)
      {
        break;
      }
        else
      {
        ret = fscanf(fp, "%04X", &address);

        if (ret == -1 || ret == EOF)
        {
          fileError();
          break;
        }

        if (start_address == -1)
          start_address = address;

        ret = fscanf(fp, "%02X", &code);

        if (ret == -1 || ret == EOF)
        {
          fileError();
          break;
        }

        if (code == 0x04)
        {
          segment = address;
        }
        else if (code == 0x00)
        {
          int checksum = 0;

          // address
          snprintf(s, sizeof(s), "S2%02X%02X%02X%02X", count + 4,
                   segment, (address >> 8) & 0xFF, address & 0xFF);
          checksum += count + 4;
          checksum += address >> 8;
          checksum += address & 0xFF;

          // data
          int index = 10;
          bool cancel = false;

          for (int i = 0; i < count; i++)
          {
            ret = fscanf(fp, "%02X", &value);

            if (ret == -1 || ret == EOF)
            {
              cancel = true;
              break;
            }

            snprintf(s + index, sizeof(s) - index, "%02X", value);
            index += 2;
            checksum += value;
          }

          if (cancel == true)
          {
            fileError();
            break;
          }

          // checksum
          snprintf(s + index, sizeof(s) - index, "%02X\n", 0xFF - (checksum & 0xFF));
          sendString(s);

          // update terminal
          can_receive = false;
          getData();
          can_receive = true;
        }
      }

      // skip to next line
      for (int i = 0; i < 256; i++)
      {
        temp = fgetc(fp);

        if (temp == '\n')
          break;
      }

      // cancel operation with escape key
      Fl::check();

      if (Gui::getCancelled() == true)
      {
        Gui::setCancelled(false);
        break;
      }
    }
  }

  snprintf(s, sizeof(s), "S804000000FB\n");
  sendString(s);

  fclose(fp);
  Gui::setAddress(start_address);
}

void Terminal::uploadSrec(const char *filename)
{
  int address = 0;
  int start_address = -1;
  int code = 0;
  int value = 0;
  int count = 0;
  int temp;
  int ret;
  char prefix[8];
  char s[256];

  FILE *fp = fopen(filename, "r");

  if (fp == NULL)
  {
    Dialog::message("Error", "Could not open file.\n");
    return;
  }

  Gui::append("\n>> Uploading Program, ESC to cancel.\n");

  while (1)
  {
    // get code from prefix
    prefix[0] = fgetc(fp);

    if (prefix[0] == EOF)
      break;

    prefix[1] = fgetc(fp);

    if (prefix[1] == EOF)
      break;

    code = prefix[1] - '0';

    if (code < 0 || code > 2)
      break;

    ret = fscanf(fp, "%02X", &count);

    if (ret == -1 || ret == EOF)
    {
      fileError();
      break;
    }

    if (code == 1)
      count -= 3;
    else if (code == 2)
      count -= 4;

    // last line
    if (count == 0)
    {
      break;
    }
    else if (code > 0)
    {
      if (code == 1)
      {
        ret = fscanf(fp, "%04X", &address);

        if (ret == -1 || ret == EOF)
        {
          fileError();
          break;
        }

        if (start_address == -1)
          start_address = address;
      }
      else if (code == 2)
      {
        ret = fscanf(fp, "%06X", &address);

        if (ret == -1 || ret == EOF)
        {
          fileError();
          break;
        }

        if (start_address == -1)
          start_address = address;
      }
        else
      {
        break;
      }

      int checksum = 0;

      // address
      snprintf(s, sizeof(s), "S2%02X%02X%02X%02X", count + 4,
               (address >> 16) & 0xFF, (address >> 8) & 0xFF, address & 0xFF);
      checksum += count + 4;
      checksum += address >> 8;
      checksum += address & 0xFF;

      // data
      int index = 10;

      for (int i = 0; i < count; i++)
      {
        ret = fscanf(fp, "%02X", &value);

        if (ret == -1 || ret == EOF)
        {
          fileError();
          break;
        }

        snprintf(s + index, sizeof(s) - index, "%02X", value);
        index += 2;
        checksum += value;
      }

      // checksum
      snprintf(s + index, sizeof(s) - index, "%02X\n",
               0xFF - (checksum & 0xFF));
      sendString(s);

      // update terminal
      can_receive = false;
      getData();
      can_receive = true;
    }

    // skip to next line
    for (int i = 0; i < 256; i++)
    {
      temp = fgetc(fp);

      if (temp == '\n')
        break;
    }

    // cancel operation with escape key
    Fl::check();

    if (Gui::getCancelled() == true)
    {
      Gui::setCancelled(false);
      break;
    }
  }

  snprintf(s, sizeof(s), "S804000000FB\n");
  sendString(s);

  fclose(fp);
  Gui::setAddress(start_address);
}

