; LED Blink - by Michael Kohn
; http://www.mikekohn.net/micro/modern_6502.php

.org 0x1000

start:

  ;; The next three lines should be removed if running on a pure
  ;; 6502 chip.  The sec / xce combination of 65C816 instructions
  ;; put the chip in 6502 mode.
.65816
  ; set 6502 mode
  sec
  xce

.65xx
  ;; Turn off interrupts to protect from ROM routines running
  sei

main:
  ;; LED off
  lda #0xff
  sta 0xdf23
  jsr delay
 
  ; LED on
  lda #0x00
  sta 0xdf23
  jsr delay

  jmp main

delay:
  ldy #0x00
delay_outer:
  ldx #0x00
delay_inner:
  dex
  bne delay_inner
  dey
  bne delay_outer
  rts

