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

#ifndef DIALOG_H
#define DIALOG_H

#ifndef PACKAGE_STRING
#define PACKAGE_STRING "EasySXB (Development Version)"
#endif

class Dialog
{
public:
  static void init();
  static void about();
  static void connect();
  static void message(const char *, const char *);
  static bool choice(const char *, const char *);

private:
  Dialog() { }
  ~Dialog() { }
};

#endif

