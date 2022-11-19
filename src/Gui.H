/*
Copyright (c) 2022 Joe Davisson.

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

#ifndef GUI_H
#define GUI_H

class Fl_Double_Window;
class Fl_Menu_Bar;

namespace Gui
{
  enum
  {
    MODE_265,
    MODE_134
  };

  void init();
  void show();
  void setMenuItem(const char *);
  void clearMenuItem(const char *);
  Fl_Double_Window *getWindow();
  Fl_Menu_Bar *getMenuBar();
  void append(const char *);
  void checkPC();
  void checkA();
  void checkX();
  void checkY();
  void checkSP();
  void checkDP();
  void checkSR();
  void checkDB();
  void checkGet();
  void checkJML();
  void checkJSL();
  void checkToggles();
  void setToggles(int);
  void updateRegs(char *);
  void flashCursor(bool);
  void setMode265();
  void setMode134();
  void setFontSmall();
  void setFontMedium();
  void setFontLarge();
  void setCancelled(bool);
  bool getCancelled();
  int getMode();
}

#endif

