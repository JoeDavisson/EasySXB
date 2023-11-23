/*
Copyright (c) 2023 Joe Davisson.

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

class Terminal
{
public:
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

  static void connect();
  static void disconnect();
  static bool isConnected();
  static void sendChar(char);
  static char getChar();
  static void sendString(const char *);
  static void getResult(char *);
  static void getData();
  static void receive(void *);
  static void changeReg(int, int);
  static void updateRegs();
  static void jml(int);
  static void jsl(int);
  static void upload();
  static void uploadHex(const char *);
  static void uploadSrec(const char *);

  static char port_string[256];

private:
  Terminal() { }
  ~Terminal() { }
};

#endif

