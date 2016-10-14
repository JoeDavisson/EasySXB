/*
Copyright (c) 2016 Joe Davisson.

This file is part of EasySXB.

EasySXB is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

EasySXB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with EasySXB; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
*/

#ifndef TERMINAL_H
#define TERMINAL_H

namespace Terminal
{
  enum
  {
    REG_PC,
    REG_A,
    REG_X,
    REG_Y,
    REG_SP,
    REG_DP,
    REG_SR,
    REG_DB
  };

  void connect();
  void disconnect();
  bool isConnected();
  void sendChar(char);
  char getChar();
  void sendString(const char *);
  void getResult(char *);
  void getData();
  void receive(void *);
  void changeReg(int, int);
  void updateRegs();
  void jml(int);
  void jsl(int);
  void upload();
  void uploadHex(const char *);
  void uploadSrec(const char *);

  extern char port_string[256];
}

#endif

