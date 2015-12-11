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

#include <cmath>
#include <typeinfo>

#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Tooltip.H>
#include <FL/Fl_Widget.H>

#include "Dialog.H"
#include "Gui.H"
#include "Separator.H"
#include "Terminal.H"

class MainWin;

namespace
{
  // window
  MainWin *window;

  // main menu
  Fl_Menu_Bar *menubar;

  Fl_Group *top;
  Fl_Group *side;

  Fl_Text_Buffer *server_text;
  Fl_Text_Display *server_display;

  Fl_Input *input_pc;
  Fl_Input *input_a;
  Fl_Input *input_x;
  Fl_Input *input_y;
  Fl_Input *input_sp;
  Fl_Input *input_dp;
  Fl_Input *input_db;

  // quit program
  void quit()
  {
    if(Dialog::choice("Quit", "Are You Sure?"))
      exit(0);
  }

  // prevent escape from closing main window
  void closeCallback(Fl_Widget *widget, void *)
  {
    if((Fl::event() == FL_KEYDOWN || Fl::event() == FL_SHORTCUT)
       && Fl::event_key() == FL_Escape)
    {
      return;
    }
    else
    {
      if(Dialog::choice("Quit", "Are You Sure?"))
        widget->hide();
    }
  }
}

// custom class to control window behavior
class MainWin : public Fl_Double_Window
{
public:
  MainWin(int w, int h, const char *label)
  : Fl_Double_Window(w, h, label)
  {
  }

  ~MainWin()
  {
  }
  
  int handle(int event)
  {
    bool shift, ctrl;

    switch(event)
    {
      case FL_FOCUS:
        return 1;
      case FL_UNFOCUS:
        return 1;
      case FL_KEYBOARD:
        // give focus to the main menu
        if(Fl::event_alt() > 0)
        {
          Gui::getMenuBar()->take_focus();
          return 0;
        }

        shift = Fl::event_shift() ? true : false;
        ctrl = Fl::event_ctrl() ? true : false;

        // misc keys
        const char *s = Fl::event_text();

        if(s[0] != 0)
        {
          Terminal::sendChar(s[0]);
        }

        return 1;
    }

    return Fl_Double_Window::handle(event);
  }
};

// initialize main gui
void Gui::init()
{
  int pos;

  // main window
  window = new MainWin(512, 384, "EasySXB");
  window->callback(closeCallback);

  // generate menu
  menubar = new Fl_Menu_Bar(0, 0, window->w(), 24);
  menubar->box(FL_THIN_UP_BOX);

  menubar->add("&File/&Connect to SXB...", 0,
    (Fl_Callback *)Dialog::connect, 0, 0);
  menubar->add("&File/&Disconnect", 0,
    (Fl_Callback *)Terminal::disconnect, 0, FL_MENU_DIVIDER);
  menubar->add("&File/&Load Program...", 0,
    (Fl_Callback *)Terminal::upload, 0, FL_MENU_DIVIDER);
  menubar->add("&File/&Quit...", 0,
    (Fl_Callback *)quit, 0, 0);

  menubar->add("&Help/&About...", 0,
    (Fl_Callback *)Dialog::about, 0, 0);

  server_text = new Fl_Text_Buffer();

  top = new Fl_Group(0, menubar->h(),
                     window->w(), window->h() - menubar->h());

  side = new Fl_Group(0, menubar->h(), 128, window->h() - menubar->h());
  pos = 8;

  input_pc = new Fl_Input(side->w() - 72 - 8, side->y() + pos, 72, 24, "PC:");
  input_pc->maximum_size(6);
  input_pc->when(FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED);
  input_pc->callback((Fl_Callback *)checkPC);
  pos += 24 + 8;
  input_a = new Fl_Input(side->w() - 72 - 8, side->y() + pos, 72, 24, "A:");
  input_a->maximum_size(4);
  input_a->when(FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED);
  input_a->callback((Fl_Callback *)checkA);
  pos += 24 + 8;
  input_x = new Fl_Input(side->w() - 72 - 8, side->y() + pos, 72, 24, "X:");
  input_x->maximum_size(4);
  input_x->when(FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED);
  input_x->callback((Fl_Callback *)checkX);
  pos += 24 + 8;
  input_y = new Fl_Input(side->w() - 72 - 8, side->y() + pos, 72, 24, "Y:");
  input_y->maximum_size(4);
  input_y->when(FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED);
  input_y->callback((Fl_Callback *)checkY);
  pos += 24 + 8;
  input_sp = new Fl_Input(side->w() - 72 - 8, side->y() + pos, 72, 24, "SP:");
  input_sp->maximum_size(4);
  input_sp->when(FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED);
  input_sp->callback((Fl_Callback *)checkSP);
  pos += 24 + 8;
  input_dp = new Fl_Input(side->w() - 72 - 8, side->y() + pos, 72, 24, "DP:");
  input_dp->maximum_size(2);
  input_dp->when(FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED);
  input_dp->callback((Fl_Callback *)checkDP);
  pos += 24 + 8;
  input_db = new Fl_Input(side->w() - 72 - 8, side->y() + pos, 72, 24, "DB:");
  input_db->maximum_size(2);
  input_db->when(FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED);
  input_db->callback((Fl_Callback *)checkDB);
  pos += 24 + 6;

  new Separator(side, 2, pos, 124, 2, "");

  side->end();

  server_display = new Fl_Text_Display(top->x() + side->w(), top->y(), top->w(), top->h());

  //server_display->wrap_mode(Fl_Text_Display::WRAP_AT_BOUNDS, 0);
  server_display->box(FL_UP_BOX);
  server_display->textsize(14);
  server_display->textfont(FL_COURIER);
  server_display->wrap_mode(Fl_Text_Display::WRAP_AT_BOUNDS, 0);
  server_display->buffer(server_text);

  top->end();

  window->size_range(512, 384, 0, 0, 0, 0, 0);
  window->resizable(top);
  window->end();

  // fix certain icons if using a light theme
  //if(Project::theme == Project::THEME_LIGHT)
  //{
  //}
}

// show the main program window (called after gui is constructed)
void Gui::show()
{
  window->show();
}

// draw checkmark next to a menu item
void Gui::setMenuItem(const char *s)
{
  Fl_Menu_Item *m;
  m = (Fl_Menu_Item *)menubar->find_item(s);

  if(m)
    m->set();
}

// remove checkmark from menu item
void Gui::clearMenuItem(const char *s)
{
  Fl_Menu_Item *m;
  m = (Fl_Menu_Item *)menubar->find_item(s);

  if(m)
    m->clear();
}

Fl_Double_Window *Gui::getWindow()
{
  return window;
}

Fl_Menu_Bar *Gui::getMenuBar()
{
  return menubar;
}

void Gui::append(const char *text)
{
  server_text->append(text);

  int lines = server_text->count_lines(0, server_text->length());

  // limit scrollback buffer to 1000 lines
  while(lines > 1000)
  {
    server_text->remove(server_text->line_start(1),
                        server_text->line_end(1) + 1);
    lines--;
  }

  // scroll display to bottom
  server_display->insert_position(server_text->length());
  server_display->show_insert_position();
}

void Gui::checkPC()
{
  Terminal::changeReg(Terminal::REG_PC, atoi(input_pc->value()));
}

void Gui::checkA()
{
  Terminal::changeReg(Terminal::REG_A, atoi(input_pc->value()));
}

void Gui::checkX()
{
  Terminal::changeReg(Terminal::REG_X, atoi(input_pc->value()));
}

void Gui::checkY()
{
  Terminal::changeReg(Terminal::REG_Y, atoi(input_pc->value()));
}

void Gui::checkSP()
{
  Terminal::changeReg(Terminal::REG_SP, atoi(input_pc->value()));
}

void Gui::checkDP()
{
  Terminal::changeReg(Terminal::REG_DP, atoi(input_pc->value()));
}

void Gui::checkDB()
{
  Terminal::changeReg(Terminal::REG_DB, atoi(input_pc->value()));
}

void Gui::updateRegs(char *s)
{
}

