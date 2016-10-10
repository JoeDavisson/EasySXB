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

#include <algorithm>

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Progress.H>
#include <FL/Fl_Widget.H>

#include "Dialog.H"
#include "DialogWindow.H"
#include "Gui.H"
#include "Terminal.H"

#if defined(_MSC_VER)
#define PACKAGE_STRING "EasySXB Development Version"
#endif

namespace About
{
  namespace Items
  {
    DialogWindow *dialog;
    Fl_Box *copyright;
    Fl_Box *info;
    Fl_Box *version;
    Fl_Button *ok;
  }

  void begin()
  {
    Items::dialog->show();
  }

  void close()
  {
    Items::dialog->hide();
  }

  void init()
  {
    int y1 = 8;
    int ww = 0, hh = 0;

    Items::dialog = new DialogWindow(384, 0, "About");
    Items::version = new Fl_Box(FL_FLAT_BOX, 0, y1, 384, 32, PACKAGE_STRING);
    Items::version->align(FL_ALIGN_INSIDE | FL_ALIGN_TOP);
    Items::version->labelsize(16);
    y1 += 32;
    Items::info = new Fl_Box(FL_FLAT_BOX, 0, y1, 384, 32,
      "A terminal emulator for use with the SXB\n"
      "line of development boards from Western Design Center.\n");
    Items::info->align(FL_ALIGN_INSIDE | FL_ALIGN_TOP);
    Items::info->labelfont(FL_HELVETICA_ITALIC);
    Items::info->labelsize(12);
    y1 += 48;
    Items::copyright = new Fl_Box(FL_FLAT_BOX, 0, y1, 384, 32,
      "Copyright (c) 2016 Joe Davisson.\n");
    Items::copyright->align(FL_ALIGN_INSIDE | FL_ALIGN_TOP);
    Items::copyright->labelsize(12);
    y1 += 32;
    Items::dialog->addOkButton(&Items::ok, &y1);
    Items::ok->callback((Fl_Callback *)close);
    Items::ok->shortcut(FL_Enter);
    Items::dialog->set_modal();
    Items::dialog->end(); 
  }
}

namespace Connect
{
  namespace Items
  {
    DialogWindow *dialog;
    Fl_Input *device;
    Fl_Button *ok;
    Fl_Button *cancel;
  }

  void begin()
  {
    if(Terminal::isConnected())
    {
      Dialog::message("Error", "Already connected.");
      return;
    }

    Items::dialog->show();
  }

  void close()
  {
    Items::dialog->hide();
    Terminal::connect();
  }

  void quit()
  {
    Items::dialog->hide();
  }

  void init()
  {
    int y1 = 8;
    int ww = 0, hh = 0;

    Items::dialog = new DialogWindow(384, 0, "Connect to SXB");
    Items::device = new Fl_Input(128, y1, 192, 24, "Device: ");
    Items::device->align(FL_ALIGN_LEFT);
    Items::device->value(Terminal::port_string);
    y1 += 48;
    Items::dialog->addOkCancelButtons(&Items::ok, &Items::cancel, &y1);
    Items::cancel->callback((Fl_Callback *)quit);
    Items::ok->callback((Fl_Callback *)close);
    Items::ok->shortcut(FL_Enter);
    Items::dialog->set_modal();
    Items::dialog->end(); 
  }
}

namespace Message
{
  namespace Items
  {
    DialogWindow *dialog;
    Fl_Box *box;
    Fl_Button *ok;
  }

  void begin(const char *title, const char *message)
  {
    Items::dialog->copy_label(title);
    Items::box->copy_label(message);
    Items::dialog->show();
  }

  void quit()
  {
    Items::dialog->hide();
  }

  void init()
  {
    int y1 = 8;

    Items::dialog = new DialogWindow(384, 0, "Error");
    Items::box = new Fl_Box(FL_FLAT_BOX, 8, 8, 384, 64, "");
    Items::box->align(FL_ALIGN_INSIDE | FL_ALIGN_TOP);
    Items::box->labelsize(14); 
    y1 += 64;
    Items::dialog->addOkButton(&Items::ok, &y1);
    Items::ok->callback((Fl_Callback *)quit);
    Items::ok->shortcut(FL_Enter);
    Items::dialog->set_modal();
    Items::dialog->end(); 
  }
}

namespace Choice
{
  bool yes = false;

  namespace Items
  {
    DialogWindow *dialog;
    Fl_Box *box;
    Fl_Button *ok;
    Fl_Button *cancel;
  }

  void begin(const char *title, const char *message)
  {
    yes = false;
    Items::dialog->copy_label(title);
    Items::box->copy_label(message);
    Items::dialog->show();
  }

  void close()
  {
    yes = true;
    Items::dialog->hide();
  }

  void quit()
  {
    yes = false;
    Items::dialog->hide();
  }

  void init()
  {
    int y1 = 8;

    Items::dialog = new DialogWindow(384, 0, "Error");
    Items::box = new Fl_Box(FL_FLAT_BOX, 8, 8, 384, 64, "");
    Items::box->align(FL_ALIGN_INSIDE | FL_ALIGN_TOP);
    Items::box->labelsize(14); 
    y1 += 64;
    Items::dialog->addOkCancelButtons(&Items::ok, &Items::cancel, &y1);
    Items::cancel->copy_label("No");
    Items::cancel->callback((Fl_Callback *)quit);
    Items::ok->copy_label("Yes");
    Items::ok->callback((Fl_Callback *)close);
    Items::ok->shortcut(FL_Enter);
    Items::dialog->set_modal();
    Items::dialog->end(); 
  }
}

void Dialog::init()
{
  About::init();
  Connect::init();
  Message::init();
  Choice::init();
}

void Dialog::about()
{
  About::begin();
}

void Dialog::connect()
{
  Connect::begin();
}

void Dialog::message(const char *title, const char *message)
{
  Message::begin(title, message);
}

bool Dialog::choice(const char *title, const char *message)
{
  Choice::begin(title, message);

  while(Choice::Items::dialog->shown())
    Fl::check();

  return Choice::yes;
}

