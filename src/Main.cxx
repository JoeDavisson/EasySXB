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

#include "FL/Fl.H"

#include <getopt.h>

#include "Dialog.H"
#include "Gui.H"
#include "Terminal.H"

namespace
{
  enum
  {
    OPTION_THEME,
    OPTION_VERSION,
    OPTION_HELP
  };

  int verbose_flag;

  struct option long_options[] =
  {
    { "theme",   required_argument, &verbose_flag, OPTION_THEME   },
    { "version", no_argument,       &verbose_flag, OPTION_VERSION },
    { "help",    no_argument,       &verbose_flag, OPTION_HELP    },
    { 0, 0, 0, 0 }
  };

  const char *help_string =
    "\nUsage: easysxb [OPTIONS] filename\n\n"
    "Options:\n"
    " --theme=dark\t\t use dark theme\n"
    " --theme=light\t\t use light theme\n"
    " --version\t\t show version\n"
    "\n";

  void setDarkTheme()
  {
    Fl::set_color(FL_BACKGROUND_COLOR, 80, 80, 80);
    Fl::set_color(FL_BACKGROUND2_COLOR, 64, 64, 64);
    Fl::set_color(FL_FOREGROUND_COLOR, 248, 248, 248);
    Fl::set_color(FL_INACTIVE_COLOR, 128, 128, 128);
    Fl::set_color(FL_SELECTION_COLOR, 248, 248, 248);
  }

  void setLightTheme()
  {
    Fl::set_color(FL_BACKGROUND_COLOR, 224, 224, 224);
    Fl::set_color(FL_BACKGROUND2_COLOR, 192, 192, 192);
    Fl::set_color(FL_FOREGROUND_COLOR, 0, 0, 0);
    Fl::set_color(FL_INACTIVE_COLOR, 128, 128, 128);
    Fl::set_color(FL_SELECTION_COLOR, 64, 64, 64);
  }
}

int main(int argc, char *argv[])
{
  // default to light theme
  setLightTheme();

  // parse command line
  int option_index = 0;

  while(true)
  {
    const int c = getopt_long(argc, argv, "", long_options, &option_index);
    if(c < 0)
      break;

    switch(c)
    {
      case 0:
      {
        switch(option_index)
        {
          case OPTION_THEME:
            if(strcmp(optarg, "dark") == 0)
            {
              setDarkTheme();
              break;
            }
            if(strcmp(optarg, "light") == 0)
            {
              setLightTheme();
              break;
            }
            printf("\nUnknown theme: \"%s\"\n", optarg);
            return 0;
          case OPTION_HELP:
            printf("%s\n", help_string);
            return 0;

          case OPTION_VERSION:
            printf("%s\n", PACKAGE_STRING);
            return 0;

          default:
            printf("%s\n", help_string);
            return 0 ;
        }

        break;
      }

      default:
      {
        printf("%s\n", help_string);
        return 0 ;
      }
    }
  }

  // fltk related inits
  Fl::visual(FL_DOUBLE | FL_RGB);
  Fl::scheme("gtk+");

  // program inits
  Dialog::init();
  Gui::init();

  // delay showing main gui until after all arguments are checked
  Gui::show();

  Fl::add_timeout(1, Terminal::receive);
  int ret = Fl::run();
  return ret;
}

