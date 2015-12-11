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
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Tooltip.H>

#include "Dialog.H"
#include "Gui.H"
#include "Terminal.H"

class MainWin;

namespace
{
  // window
  MainWin *window;

  // main menu
  Fl_Menu_Bar *menubar;

  Fl_Group *top;

  Fl_Text_Buffer *server_text;
  Fl_Text_Display *server_display;

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
          Terminal::send(s[0]);
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

  server_display = new Fl_Text_Display(top->x(), top->y(), top->w(), top->h());

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


