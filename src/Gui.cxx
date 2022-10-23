/*
Copyright (c) 2016 Joe Davisson.

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

// test comment

#include <cmath>
#include <cstdlib>
#include <typeinfo>

#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Light_Button.H>
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
  int mode = Gui::MODE_265;
  bool cancelled = false;

  MainWin *window;
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
  Fl_Input *input_sr;
  Fl_Input *input_db;
  Fl_Button *button_get;

  Fl_Input *input_address;
  Fl_Button *button_jml;
  Fl_Button *button_jsl;

  Fl_Light_Button *light_n;
  Fl_Light_Button *light_v;
  Fl_Light_Button *light_m;
  Fl_Light_Button *light_x;
  Fl_Light_Button *light_d;
  Fl_Light_Button *light_i;
  Fl_Light_Button *light_z;
  Fl_Light_Button *light_c;

  // quit program
  void quit()
  {
    if(Dialog::choice("Quit", "Are You Sure?"))
    {
      Gui::setCancelled(true);
      exit(0);
    }
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
        if(Fl::event_key() == FL_Escape)
        {
          Gui::setCancelled(true);
          return 1;
        }

        // give focus to the main menu
        if(Fl::event_alt() > 0)
        {
          Gui::getMenuBar()->take_focus();
          return 0;
        }

        shift = Fl::event_shift() ? true : false;
        ctrl = Fl::event_ctrl() ? true : false;

        // misc keys
        if(Fl::event_length() > 0)
        {
          Terminal::sendString(Fl::event_text());
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
  window = new MainWin(768, 480, "EasySXB");
  window->callback(closeCallback);

  // generate menu
  menubar = new Fl_Menu_Bar(0, 0, window->w(), 24);
  menubar->box(FL_THIN_UP_BOX);

  menubar->add("&File/&Connect to SXB...", 0,
    (Fl_Callback *)Dialog::connect, 0, 0);
  menubar->add("&File/&Disconnect", 0,
    (Fl_Callback *)Terminal::disconnect, 0, FL_MENU_DIVIDER);
  menubar->add("&File/&Upload Program...", 0,
    (Fl_Callback *)Terminal::upload, 0, FL_MENU_DIVIDER);
  menubar->add("&File/&Quit...", 0,
    (Fl_Callback *)quit, 0, 0);

  menubar->add("&Options/&Board Model/W65C265SXB", 0,
    (Fl_Callback *)setMode265, 0, FL_MENU_RADIO);
  menubar->add("&Options/&Board Model/W65C134SXB", 0,
    (Fl_Callback *)setMode134, 0, FL_MENU_RADIO | FL_MENU_DIVIDER);
  menubar->add("&Options/&Font Size/Small", 0,
    (Fl_Callback *)setFontSmall, 0, FL_MENU_RADIO);
  menubar->add("&Options/&Font Size/Medium", 0,
    (Fl_Callback *)setFontMedium, 0, FL_MENU_RADIO);
  menubar->add("&Options/&Font Size/Large", 0,
    (Fl_Callback *)setFontLarge, 0, FL_MENU_RADIO);

  setMenuItem("&Options/&Board Model/W65C265SXB");
  setMenuItem("&Options/&Font Size/Medium");

  menubar->add("&Help/&About...", 0,
    (Fl_Callback *)Dialog::about, 0, 0);

  server_text = new Fl_Text_Buffer();

  top = new Fl_Group(0, menubar->h(),
                     window->w(), window->h() - menubar->h());

  side = new Fl_Group(0, menubar->h(), 128, window->h() - menubar->h());
  pos = side->y() + 8;

  input_pc = new Fl_Input(side->w() - 72 - 8, pos, 72, 20, "PC:");
  input_pc->textfont(FL_COURIER);
  input_pc->labelfont(FL_COURIER);
  input_pc->maximum_size(6);
  input_pc->when(FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED);
  input_pc->callback((Fl_Callback *)checkPC);
  pos += 20 + 4;
  input_a = new Fl_Input(side->w() - 72 - 8, pos, 48, 20, "A:");
  input_a->textfont(FL_COURIER);
  input_a->labelfont(FL_COURIER);
  input_a->maximum_size(4);
  input_a->when(FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED);
  input_a->callback((Fl_Callback *)checkA);
  pos += 20 + 4;
  input_x = new Fl_Input(side->w() - 72 - 8, pos, 48, 20, "X:");
  input_x->textfont(FL_COURIER);
  input_x->labelfont(FL_COURIER);
  input_x->maximum_size(4);
  input_x->when(FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED);
  input_x->callback((Fl_Callback *)checkX);
  pos += 20 + 4;
  input_y = new Fl_Input(side->w() - 72 - 8, pos, 48, 20, "Y:");
  input_y->textfont(FL_COURIER);
  input_y->labelfont(FL_COURIER);
  input_y->maximum_size(4);
  input_y->when(FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED);
  input_y->callback((Fl_Callback *)checkY);
  pos += 20 + 4;
  input_sp = new Fl_Input(side->w() - 72 - 8, pos, 48, 20, "SP:");
  input_sp->textfont(FL_COURIER);
  input_sp->labelfont(FL_COURIER);
  input_sp->maximum_size(4);
  input_sp->when(FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED);
  input_sp->callback((Fl_Callback *)checkSP);
  pos += 20 + 4;
  input_dp = new Fl_Input(side->w() - 72 - 8, pos, 48, 20, "DP:");
  input_dp->textfont(FL_COURIER);
  input_dp->labelfont(FL_COURIER);
  input_dp->maximum_size(4);
  input_dp->when(FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED);
  input_dp->callback((Fl_Callback *)checkDP);
  pos += 20 + 4;
  input_sr = new Fl_Input(side->w() - 72 - 8, pos, 24, 20, "SR:");
  input_sr->textfont(FL_COURIER);
  input_sr->labelfont(FL_COURIER);
  input_sr->maximum_size(2);
  input_sr->when(FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED);
  input_sr->callback((Fl_Callback *)checkSR);
  pos += 20 + 4;
  input_db = new Fl_Input(side->w() - 72 - 8, pos, 24, 20, "DB:");
  input_db->textfont(FL_COURIER);
  input_db->labelfont(FL_COURIER);
  input_db->maximum_size(2);
  input_db->when(FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED);
  input_db->callback((Fl_Callback *)checkDB);
  pos += 20 + 4;
  button_get = new Fl_Button(16, pos, 96, 24, "Get");
  button_get->labelfont(FL_COURIER);
  button_get->callback((Fl_Callback *)checkGet);
  pos += 24 + 6;

  new Separator(side, 2, pos - side->y(), 124, 2, "");
  pos += 8;

  input_address = new Fl_Input(side->w() - 60 - 8, pos, 60, 20, "Address:");
  input_address->textfont(FL_COURIER);
  input_address->labelfont(FL_COURIER);
  input_address->labelsize(10);
  input_address->maximum_size(6);
  pos += 24 + 8;
  button_jml = new Fl_Button(8, pos, 52, 24, "JML");
  button_jml->labelfont(FL_COURIER);
  button_jml->callback((Fl_Callback *)checkJML);
  button_jsl = new Fl_Button(68, pos, 52, 24, "JSL");
  button_jsl->labelfont(FL_COURIER);
  button_jsl->callback((Fl_Callback *)checkJSL);
  pos += 24 + 6;

  new Separator(side, 2, pos - side->y(), 124, 2, "");
  pos += 8;

  light_n = new Fl_Light_Button(8, pos, 112, 14, "(N) Negative");
  light_n->labelfont(FL_COURIER);
  light_n->labelsize(10);
  light_n->box(FL_NO_BOX);
  light_n->down_box(FL_OVAL_BOX);
  light_n->down_color(fl_rgb_color(0, 192, 0));
  light_n->callback((Fl_Callback *)checkToggles);
  pos += 14 + 2;
  light_v = new Fl_Light_Button(8, pos, 112, 14, "(V) Overflow");
  light_v->labelfont(FL_COURIER);
  light_v->labelsize(10);
  light_v->box(FL_NO_BOX);
  light_v->down_box(FL_OVAL_BOX);
  light_v->down_color(fl_rgb_color(0, 192, 0));
  light_v->callback((Fl_Callback *)checkToggles);
  pos += 14 + 2;
  light_m = new Fl_Light_Button(8, pos, 112, 14, "(M) A = 8-bit");
  light_m->labelfont(FL_COURIER);
  light_m->labelsize(10);
  light_m->box(FL_NO_BOX);
  light_m->down_box(FL_OVAL_BOX);
  light_m->down_color(fl_rgb_color(0, 192, 0));
  light_m->callback((Fl_Callback *)checkToggles);
  pos += 14 + 2;
  light_x = new Fl_Light_Button(8, pos, 112, 14, "(X) X/Y = 8-bit");
  light_x->labelfont(FL_COURIER);
  light_x->labelsize(10);
  light_x->box(FL_NO_BOX);
  light_x->down_box(FL_OVAL_BOX);
  light_x->down_color(fl_rgb_color(0, 192, 0));
  light_x->callback((Fl_Callback *)checkToggles);
  pos += 14 + 2;
  light_d = new Fl_Light_Button(8, pos, 112, 14, "(D) Decimal Mode");
  light_d->labelfont(FL_COURIER);
  light_d->labelsize(10);
  light_d->box(FL_NO_BOX);
  light_d->down_box(FL_OVAL_BOX);
  light_d->down_color(fl_rgb_color(0, 192, 0));
  light_d->callback((Fl_Callback *)checkToggles);
  pos += 14 + 2;
  light_i = new Fl_Light_Button(8, pos, 112, 14, "(I) IRQ Disable");
  light_i->labelfont(FL_COURIER);
  light_i->labelsize(10);
  light_i->box(FL_NO_BOX);
  light_i->down_box(FL_OVAL_BOX);
  light_i->down_color(fl_rgb_color(0, 192, 0));
  light_i->callback((Fl_Callback *)checkToggles);
  pos += 14 + 2;
  light_z = new Fl_Light_Button(8, pos, 112, 14, "(Z) Zero");
  light_z->labelfont(FL_COURIER);
  light_z->labelsize(10);
  light_z->box(FL_NO_BOX);
  light_z->down_box(FL_OVAL_BOX);
  light_z->down_color(fl_rgb_color(0, 192, 0));
  light_z->callback((Fl_Callback *)checkToggles);
  pos += 14 + 2;
  light_c = new Fl_Light_Button(8, pos, 112, 14, "(C) Carry");
  light_c->labelfont(FL_COURIER);
  light_c->labelsize(10);
  light_c->box(FL_NO_BOX);
  light_c->down_box(FL_OVAL_BOX);
  light_c->down_color(fl_rgb_color(0, 192, 0));
  light_c->callback((Fl_Callback *)checkToggles);
  pos += 14 + 2;

  side->resizable(0);
  side->end();

  server_display = new Fl_Text_Display(top->x() + side->w(), top->y(),
                                       top->w() - side->w(), top->h());

  server_display->box(FL_UP_BOX);
  server_display->scrollbar_width(18);
  server_display->textsize(14);
  server_display->textfont(FL_COURIER);
  server_display->wrap_mode(Fl_Text_Display::WRAP_AT_BOUNDS, 0);
  server_display->buffer(server_text);
  server_display->cursor_style(Fl_Text_Display::BLOCK_CURSOR);
  server_display->show_cursor();

  top->resizable(server_display);
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

void Gui::append(const char *buf)
{
  if(strlen(buf) < 1)
    return;

  char text[4096];
  memcpy(text, buf, sizeof(text));
  
  // convert carriage returns
  for(int i = 0; i < sizeof(text); i++)
  {
    if(text[i] == '\0')
      break;

    if(text[i] == 13)
      text[i] = '\n';
  }

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
  Fl::check();
}

void Gui::checkPC()
{
  int num;
  sscanf(input_pc->value(), "%06X", &num);
  Terminal::changeReg(Terminal::REG_PC, num);
}

void Gui::checkA()
{
  int num;
  sscanf(input_a->value(), "%04X", &num);
  Terminal::changeReg(Terminal::REG_A, num);
}

void Gui::checkX()
{
  int num;
  sscanf(input_x->value(), "%04X", &num);
  Terminal::changeReg(Terminal::REG_X, num);
}

void Gui::checkY()
{
  int num;
  sscanf(input_y->value(), "%04X", &num);
  Terminal::changeReg(Terminal::REG_Y, num);
}

void Gui::checkSP()
{
  int num;
  sscanf(input_sp->value(), "%04X", &num);
  Terminal::changeReg(Terminal::REG_SP, num);
}

void Gui::checkDP()
{
  int num;
  sscanf(input_dp->value(), "%04X", &num);
  Terminal::changeReg(Terminal::REG_DP, num);
}

void Gui::checkSR()
{
  int num;
  sscanf(input_sr->value(), "%02X", &num);
  Terminal::changeReg(Terminal::REG_SR, num);
}

void Gui::checkDB()
{
  int num;
  sscanf(input_db->value(), "%02X", &num);
  Terminal::changeReg(Terminal::REG_DB, num);
}

void Gui::checkGet()
{
  Terminal::updateRegs();
}

void Gui::checkJML()
{
  int address;
  sscanf(input_address->value(), "%06X", &address);
  Terminal::jml(address);
}

void Gui::checkJSL()
{
  int address;
  sscanf(input_address->value(), "%06X", &address);
  Terminal::jsl(address);
}

void Gui::checkToggles()
{
  int num = 0;

  num |= light_n->value();
  num <<= 1;
  num |= light_v->value();
  num <<= 1;
  num |= light_m->value();
  num <<= 1;
  num |= light_x->value();
  num <<= 1;
  num |= light_d->value();
  num <<= 1;
  num |= light_i->value();
  num <<= 1;
  num |= light_z->value();
  num <<= 1;
  num |= light_c->value();

  Terminal::changeReg(Terminal::REG_SR, num);
}

void Gui::setToggles(int num)
{
  light_n->value((num >> 7) & 1);
  light_v->value((num >> 6) & 1);
  light_m->value((num >> 5) & 1);
  light_x->value((num >> 4) & 1);
  light_d->value((num >> 3) & 1);
  light_i->value((num >> 2) & 1);
  light_z->value((num >> 1) & 1);
  light_c->value((num >> 0) & 1);
}

void Gui::updateRegs(char *s)
{
  int pc, a, x, y, sp, dp, sr, db;
  char buf[256];

  if(mode == MODE_265)
  {
    sscanf(s, "  %06X %04X %04X %04X %04X %04X %02X %02X",
           &pc, &a, &x, &y, &sp, &dp, &sr, &db);

    snprintf(buf, sizeof(buf), "%06X", pc);
    input_pc->value(buf);

    snprintf(buf, sizeof(buf), "%04X", a);
    input_a->value(buf);

    snprintf(buf, sizeof(buf), "%04X", x);
    input_x->value(buf);

    snprintf(buf, sizeof(buf), "%04X", y);
    input_y->value(buf);

    snprintf(buf, sizeof(buf), "%04X", sp);
    input_sp->value(buf);

    snprintf(buf, sizeof(buf), "%02X", dp);
    input_dp->value(buf);

    snprintf(buf, sizeof(buf), "%02X", sr);
    input_sr->value(buf);

    snprintf(buf, sizeof(buf), "%02X", db);
    input_db->value(buf);

    setToggles(sr);
  }
  else if(mode == MODE_134)
  {
    sscanf(s + 20, "%04X %02X %02X %02X %02X %02X",
           &pc, &sr, &a, &x, &y, &sp);

    snprintf(buf, sizeof(buf), "%04X", pc);
    input_pc->value(buf);

    snprintf(buf, sizeof(buf), "%02X", sr);
    input_sr->value(buf);

    snprintf(buf, sizeof(buf), "%02X", a);
    input_a->value(buf);

    snprintf(buf, sizeof(buf), "%02X", x);
    input_x->value(buf);

    snprintf(buf, sizeof(buf), "%02X", y);
    input_y->value(buf);

    snprintf(buf, sizeof(buf), "%02X", sp);
    input_sp->value(buf);

    setToggles(sr);
  }
}

void Gui::flashCursor(bool show)
{
  if(show == true)
    server_display->show_cursor();
  else
    server_display->hide_cursor();
}

void Gui::setMode265()
{
  button_jml->label("JML");
  button_jsl->label("JSL");
  light_x->label("(X) X/Y = 8-bit");
  light_m->label("(M) A = 8-bit");
  light_m->activate();
  input_pc->resize(input_pc->x(), input_pc->y(), 72, 20);
  input_a->resize(input_a->x(), input_a->y(), 48, 20);
  input_x->resize(input_x->x(), input_x->y(), 48, 20);
  input_y->resize(input_y->x(), input_y->y(), 48, 20);
  input_sp->resize(input_sp->x(), input_sp->y(), 48, 20);
  input_dp->resize(input_dp->x(), input_dp->y(), 48, 20);
  input_sr->resize(input_sr->x(), input_sr->y(), 24, 20);
  input_db->resize(input_db->x(), input_db->y(), 24, 20);
  input_address->resize(input_address->x(), input_address->y(), 60, 20);

  input_pc->maximum_size(6);
  input_a->maximum_size(4);
  input_x->maximum_size(4);
  input_y->maximum_size(4);
  input_sp->maximum_size(4);
  input_dp->maximum_size(4);
  input_sr->maximum_size(2);
  input_db->maximum_size(2);
  input_address->maximum_size(6);

  input_pc->value("");
  input_a->value("");
  input_x->value("");
  input_y->value("");
  input_sp->value("");
  input_dp->value("");
  input_sr->value("");
  input_db->value("");
  input_address->value("");

  input_dp->activate();
  input_db->activate();

  mode = MODE_265;
  window->redraw();
}

void Gui::setMode134()
{
  button_jml->label("JMP");
  button_jsl->label("JSR");
  light_x->label("(B) Break");
  light_m->label("    Unused");
  light_m->deactivate();
  input_pc->resize(input_pc->x(), input_pc->y(), 48, 20);
  input_a->resize(input_a->x(), input_a->y(), 24, 20);
  input_x->resize(input_x->x(), input_x->y(), 24, 20);
  input_y->resize(input_y->x(), input_y->y(), 24, 20);
  input_sp->resize(input_sp->x(), input_sp->y(), 24, 20);
  input_dp->resize(input_dp->x(), input_dp->y(), 24, 20);
  input_sr->resize(input_sr->x(), input_sr->y(), 24, 20);
  input_db->resize(input_db->x(), input_db->y(), 24, 20);
  input_address->resize(input_address->x(), input_address->y(), 48, 20);

  input_pc->maximum_size(4);
  input_a->maximum_size(2);
  input_x->maximum_size(2);
  input_y->maximum_size(2);
  input_sp->maximum_size(2);
  input_dp->maximum_size(2);
  input_sr->maximum_size(2);
  input_db->maximum_size(2);
  input_address->maximum_size(4);

  input_pc->value("");
  input_a->value("");
  input_x->value("");
  input_y->value("");
  input_sp->value("");
  input_dp->value("");
  input_sr->value("");
  input_db->value("");
  input_address->value("");

  input_dp->deactivate();
  input_db->deactivate();

  mode = MODE_134;
  window->redraw();
}

void Gui::setFontSmall()
{
  server_display->textsize(10);
  server_display->buffer(0);
  server_display->buffer(server_text);
  server_display->redraw();
}

void Gui::setFontMedium()
{
  server_display->textsize(14);
  server_display->buffer(0);
  server_display->buffer(server_text);
  server_display->redraw();
}

void Gui::setFontLarge()
{
  server_display->textsize(18);
  server_display->buffer(0);
  server_display->buffer(server_text);
  server_display->redraw();
}

void Gui::setCancelled(bool value)
{
  cancelled = value;
}

bool Gui::getCancelled()
{
  return cancelled;
}

int Gui::getMode()
{
  return mode;
}

