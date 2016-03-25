
import net.mikekohn.java_grinder.W65C265SXB;

public class ToneGenerator
{
  static public void main(String[] args)
  {
    int data, note;
    int tg0 = 0, tg1 = 0;
    int volume;
    int channel;

    W65C265SXB.controlTones(508, 508, true, true);

    while(true)
    {
      data = W65C265SXB.getChar();
      //W65C265SXB.putChar((char)data);

      if ((data & 0x80) == 0) { continue; }

      note = W65C265SXB.getChar();

      volume = data & 0xf;
      channel = (data >> 4) & 0x1;

      if (channel == 0)
      {
        if (volume == 0)
        {
          tg0 = 0;
        }
          else
        {
          tg0 = notes[note];
        }
      }
        else
      {
        if (volume == 0)
        {
          tg1 = 0;
        }
          else
        {
          tg1 = notes[note];
        }
      }

      W65C265SXB.controlTones(tg0, tg1, tg0 != 0, tg1 != 0);
    }
  }

  static short[] notes =
  {
    0, 25827, 24378, 23010, 21718, 20499, 19349, 18263,
    17238, 16270, 15357, 14495, 13681, 12913, 12189, 11505,
    10859, 10249, 9674, 9131, 8619, 8135, 7678, 7247,
    6840, 6456, 6094, 5752, 5429, 5124, 4837, 4565,
    4309, 4067, 3839, 3623, 3420, 3228, 3047, 2876,
    2714, 2562, 2418, 2282, 2154, 2033, 1919, 1811,
    1710, 1614, 1523, 1438, 1357, 1281, 1209, 1141,
    1077, 1016, 959, 905, 855, 807, 761, 719,
    678, 640, 604, 570, 538, 508, 479, 452,
    427, 403, 380, 359, 339, 320, 302, 285,
    269, 254, 239, 226, 213, 201, 190, 179,
    169, 160, 151, 142, 134, 127, 119, 113,
    106, 100, 95, 89, 84, 80, 75, 71,
    67, 63, 59, 56, 53, 50, 47, 44,
    42, 40, 37, 35, 33, 31, 29, 28,
    26, 25, 23, 22, 21, 20, 18,
  };
}

