#!/usr/bin/env python

notes = { }

fp = open("freq_chart.txt","rb")

for line in fp:
  tokens = line.split()

  index = 1

  while index < len(tokens):
    num = int(tokens[index])
    freq = float(tokens[index + 1])

    notes[num] = int((3579545 / 16) / freq)
    index += 2

fp.close()

s = ""

print "  short[] notes = {"
for num in range(0, 127):
  if ((num % 8) == 0): s += "\n    "
  s += str(notes[num]) + ", "

print s
print "}"



