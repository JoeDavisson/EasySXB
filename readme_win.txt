EasySXB for Windows
by Joe Davisson


Introduction
------------
EasySXB is a terminal emulator designed for use with the SXB line of board-level products from Western Design Center. Please visit http://wdc65xx.com/ for more information.

The following products are currently supported:
* W65C265SXB
* W65C134SXB


Getting Started
---------------
First, obtain the port name from Device Manager (Usually COM3, COM4, etc). You may have to re-check this if you reconnect the board or reboot the system. Before connecting, choose the board type from the "Mode" menu.

The interface on the left provides quick access to some of the build-in monitor functions. You can alter the registers, jump to subroutines, and toggle the flags. The appropriate commands are sent to the monitor automatically. Use the "Get" button to keep things up-to-date.

Note: You can use the SXB's reset button without losing the connection.


Uploading Programs
------------------
Currently only the Intel HEX format is supported. The monitor will provide feedback as each line is processed. The Flash ROM socket is not yet supported.


Source Code
-----------
The source is available here:
https://github.com/Mortis69/EasySXB


Support
-------
Contact me at: joe_7@sbcglobal.net

