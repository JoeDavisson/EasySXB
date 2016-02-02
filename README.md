EasySXB
=======

![Screenshot](https://raw.githubusercontent.com/Mortis69/EasySXB/master/screenshots/screenshot.png)

EasySXB is a terminal/program loader designed for use with Western Design Center's SXB line of development boards. Currently the W65C265SXB and W65C134SXB products are supported.

An interactive interface is provided for commonly-used options. Programs (in HEX format) may also be uploaded to the boards.

The user must have serial port access rights. Windows and Linux binaries are available here:

http://sourceforge.net/projects/easysxb/

## Build it from source
```$ git clone https://github.com/Mortis69/EasySXB.git```

```$ cd EasySXB```

Uncompress the FLTK-1.3.3 source package here.

The Makefile supports ```linux``` and ```mingw``` cross-compiler targets.
(Edit the Makefile to choose.)

```$ make fltk```

```$ make```

## Dependencies

### Libraries

 * FLTK-1.3.3
 * libxft-dev (required for font rendering)

