.65816
.org 0x1000

start:
  ; set native mode
  clc
  xce

  ; set A/X/Y to 8-bit
  sep #0x30

main:
  ;; LED off
  lda.b #0xff
  sta 0xdf23
  jsr delay
 
  ;; LED on
  lda.b #0x00
  sta 0xdf23
  jsr delay

  jmp main

delay:
  ldy.b #0
delay_outer:
  ldx.b #0
delay_inner:
  dex
  bne delay_inner
  dey
  bne delay_outer
  rts

