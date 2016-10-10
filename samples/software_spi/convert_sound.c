#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

int read_int32(FILE *in)
{
  int i;

  i = getc(in);
  i |= getc(in)<<8;
  i |= getc(in)<<16;
  i |= getc(in)<<24;
 
  return i;
}

int read_int16(FILE *in)
{
  int i;

  i = getc(in);
  i |= getc(in)<<8;

  return i;
}

void write_int32(FILE *out, unsigned int i)
{
  putc((i >> 24), out);
  putc((i >> 16) & 255, out);
  putc((i >> 8) & 255, out);
  putc(i & 255, out);
}

void write_int16(FILE *out, unsigned int i)
{
  putc((i >> 8) & 255, out);
  putc(i & 255, out);
}

uint32_t add_file(FILE *out, FILE *in)
{
  char chunk_name[5];
  int chunk_size;
  char format[5];
  int n;
  int extra_len = 0;
  int16_t sample;
  int amp_min = 32768;
  int amp_max = -32768;
  int sample_min = 4096;
  int sample_max = 0;

  chunk_name[4] = 0;
  format[4] = 0;

  printf("---------------------------------\n");

  for(n = 0; n < 4; n++) { chunk_name[n] = getc(in); }
  chunk_size = read_int32(in);
  for(n = 0; n < 4; n++) { format[n] = getc(in); }
  printf("%s\n%d\n%s\n\n", chunk_name, chunk_size, format);

  for(n = 0; n < 4; n++) { chunk_name[n] = getc(in); }
  chunk_size = read_int32(in);
  printf("%s\n%d\n\n", chunk_name, chunk_size);

  printf("   Audio Format: %d\n", read_int16(in));
  printf("   Num channels: %d\n", read_int16(in));
  printf("    Sample Rate: %d\n", read_int32(in));
  printf("      Byte Rate: %d\n", read_int32(in));
  printf("    Block Align: %d\n", read_int16(in));
  printf("Bits Per Sample: %d\n", read_int16(in));

  for(n = 0; n < 4; n++) { chunk_name[n] = getc(in); }
  chunk_size = read_int32(in);
  printf("%s\n%d\n\n", chunk_name, chunk_size);

  for(n = 0; n < chunk_size; n = n + 2)
  {
    sample = read_int16(in);
    int s32 = sample;
    if (s32 < amp_min) { amp_min = s32; }
    if (s32 > amp_max) { amp_max = s32; }
    s32 += 32768;
    uint32_t us32 = s32;
    us32 = us32 >> 4;

    //printf("%04x  %d %d\n",(unsigned short int)sample, sample, us32);
    if (us32 < sample_min) { sample_min = us32; }
    if (us32 > sample_max) { sample_max = us32; }

    us32 = us32 | 0x3000;

    write_int16(out, us32);
    write_int16(out, us32);
  }

  write_int16(out, 0x3800);
  write_int16(out, 0x0000);

  printf("amp_min=%d\n", amp_min);
  printf("amp_max=%d\n", amp_max);
  printf("sample_min=%d\n", sample_min);
  printf("sample_max=%d\n", sample_max);

  return chunk_size+extra_len;
}

int main(int argc, char *argv[])
{
  FILE *out, *in;

  if (argc != 2)
  {
    printf("Usage: %s <infile.wav>\n", argv[0]);
    exit(0);
  }

  out = fopen("sound.img", "wb");

  if (out == NULL)
  {
    printf("Couldn't open sound.img for writing.\n");
    exit(1);
  }

  in = fopen(argv[1], "rb");

  if (in == NULL)
  {
    printf("Couldn't open %s for reading.\n", argv[1]);
    exit(1);
  }

  add_file(out, in);

  fclose(out);
  fclose(in);

  return 0;
}

