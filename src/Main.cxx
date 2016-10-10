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
#include <FL/Fl_Native_File_Chooser.H>

#if defined(_MSC_VER)
#define PACKAGE_STRING "EasySXB Development Version"

struct option		/* specification for a long form option...	*/
{
  const char *name;		  /* option name, without leading hyphens */
  int         has_arg;	/* does it take an argument?		*/
  int        *flag;		  /* where to save its status, or NULL	*/
  int         val;		  /* its associated status value		*/
};

enum    		/* permitted values for its `has_arg' field...	*/
{
  no_argument = 0,    /* option never takes an argument	*/
  required_argument,	/* option always requires an argument	*/
  optional_argument		/* option may take an argument		*/
};
#else
#include <getopt.h>
#endif

#include "Dialog.H"
#include "Gui.H"
#include "Terminal.H"

namespace
{
  enum
  {
    OPTION_PORT,
    OPTION_FILE,
    OPTION_THEME,
    OPTION_VERSION,
    OPTION_HELP
  };

  int verbose_flag;

  struct option long_options[] =
  {
    { "port",      required_argument, &verbose_flag, OPTION_PORT   },
    { "file",      required_argument, &verbose_flag, OPTION_FILE   },
    { "theme",     required_argument, &verbose_flag, OPTION_THEME   },
    { "version",   no_argument,       &verbose_flag, OPTION_VERSION },
    { "help",      no_argument,       &verbose_flag, OPTION_HELP    },
    { 0, 0, 0, 0 }
  };

  const char *help_string =
    "\nUsage: easysxb [OPTIONS] filename\n\n"
    "Options:\n"
    " --port        specify port\n"
    " --file        connect and upload .hex or .srec file\n"
    " --theme       select theme (light or dark)\n"
    " --version     show version\n"
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
  char file_string[1024];
  bool upload = false;

#ifdef WIN32
  strcpy(Terminal::port_string, "COM1");
#else
  strcpy(Terminal::port_string, "/dev/ttyUSB0");
#endif

#if !defined(_MSC_VER)
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
          case OPTION_PORT:
            strncpy(Terminal::port_string, optarg, 256);
            break;
          case OPTION_FILE:
            strncpy(file_string, optarg, 1024);
            upload = true;
            break;
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
#endif

  // fltk related inits
  Fl::visual(FL_DOUBLE | FL_RGB);
  Fl::scheme("gtk+");

  // program inits
  Dialog::init();
  Gui::init();

  // delay showing main gui until after all arguments are checked
  Gui::show();
  Fl::add_timeout(1, Terminal::receive);

  // upload a file?
  if(upload == true)
  {
    Terminal::connect();

    const char *ext = fl_filename_ext(file_string);
  
    if(strcasecmp(ext, ".hex") == 0)
      Terminal::uploadHex(file_string);
    else if(strcasecmp(ext, ".srec") == 0)
      Terminal::uploadSrec(file_string);
    else Dialog::message("Upload Error", "Only .hex and .srec file extentions are supported.");
  }

  int ret = Fl::run();
  return ret;
}

