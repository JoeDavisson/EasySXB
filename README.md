EasySXB
=======

![Screenshot](https://raw.githubusercontent.com/JoeDavisson/EasySXB/master/screenshots/screenshot.png)

EasySXB is a terminal and program loader designed for use with Western Design Center's SXB line of development boards. The W65C265SXB, W65C134SXB, and W65C265QBX products are currently supported.

An interactive interface is provided for commonly-used options. Programs (in Intel HEX or Motorola S-Record format) may also be uploaded to the boards.

The user must have serial port access rights (in Linux add the user to the ```dialout``` group). 

Version 0.1.7 fixes compiler warnings and allows DPI scaling with ```ctrl +/-/0```.

## Build it from source
```$ git clone https://github.com/JoeDavisson/EasySXB.git```

```$ cd EasySXB```

Edit FLTK_DIR and PLATFORM in the Makefile, then build FLTK:

```$ make fltklib```

Build EasySXB:

```$ make```

## Dependencies
 * [naken_asm](https://github.com/mikeakohn/naken_asm) and [java_grinder](https://github.com/mikeakohn/java_grinder) (for building the samples)

### Libraries

 * FLTK-1.4.5 (or later)
 * libxft-dev (required for font rendering on Linux)

