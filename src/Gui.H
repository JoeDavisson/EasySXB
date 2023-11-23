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

#ifndef GUI_H
#define GUI_H

class Fl_Double_Window;
class Fl_Menu_Bar;

class Gui
{
public:
  enum
  {
    MODE_265,
    MODE_134
  };

  static void init();
  static void show();
  static void setMenuItem(const char *);
  static void clearMenuItem(const char *);
  static Fl_Double_Window *getWindow();
  static Fl_Menu_Bar *getMenuBar();
  static void append(const char *);
  static void checkPC();
  static void checkA();
  static void checkX();
  static void checkY();
  static void checkSP();
  static void checkDP();
  static void checkSR();
  static void checkDB();
  static void checkUpdate();
  static void checkJML();
  static void checkJSL();
  static void checkToggles();
  static void setToggles(int);
  static void updateRegs(char *);
  static void flashCursor(bool);
  static void setMode265();
  static void setMode134();
  static void setFontSmall();
  static void setFontMedium();
  static void setFontLarge();
  static void setCancelled(bool);
  static bool getCancelled();
  static int getMode();

private:
  Gui() { }
  ~Gui() { }
};

#endif

