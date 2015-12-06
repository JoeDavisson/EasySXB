/*
Copyright (c) 2015 Joe Davisson.

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

#include "FL/Fl.H"
//#include "FL/Fl_Timeout_Handler.H"

#include "Dialog.H"
#include "Gui.H"

namespace
{
  void setDarkTheme()
  {
//    Project::theme = Project::THEME_DARK;
    Fl::set_color(FL_BACKGROUND_COLOR, 80, 80, 80);
    Fl::set_color(FL_BACKGROUND2_COLOR, 64, 64, 64);
    Fl::set_color(FL_FOREGROUND_COLOR, 248, 248, 248);
    Fl::set_color(FL_INACTIVE_COLOR, 128, 128, 128);
    Fl::set_color(FL_SELECTION_COLOR, 248, 248, 248);
//    Project::theme_highlight_color = makeRgb(0, 64, 128);
//    const int blend = Blend::lighten(makeRgb(80, 80, 80),
//                                     Project::theme_highlight_color, 96);
//    Project::fltk_theme_highlight_color = fl_rgb_color(getr(blend),
//                                                       getg(blend),
 //                                                      getb(blend));
  }

  void setLightTheme()
  {
//    Project::theme = Project::THEME_LIGHT;
    Fl::set_color(FL_BACKGROUND_COLOR, 224, 224, 224);
    Fl::set_color(FL_BACKGROUND2_COLOR, 192, 192, 192);
    Fl::set_color(FL_FOREGROUND_COLOR, 0, 0, 0);
    Fl::set_color(FL_INACTIVE_COLOR, 128, 128, 128);
    Fl::set_color(FL_SELECTION_COLOR, 64, 64, 64);
//    Project::theme_highlight_color = makeRgb(0, 64, 128);
//    const int blend = Blend::darken(makeRgb(224, 224, 224),
//                                    Project::theme_highlight_color, 96);
//    Project::fltk_theme_highlight_color = fl_rgb_color(getr(blend),
//                                                       getg(blend),
//                                                       getb(blend));
  }
}

int main(int argc, char *argv[])
{
  // default to a nice dark theme
  setDarkTheme();

  // fltk related inits
  Fl::visual(FL_DOUBLE | FL_RGB);
  Fl::scheme("gtk+");

  // program inits
  Dialog::init();
  Gui::init();

  // delay showing main gui until after all arguments are checked
  Gui::show();

//  Fl::add_timeout(1, Chat::read);
  int ret = Fl::run();
  return ret;
}

