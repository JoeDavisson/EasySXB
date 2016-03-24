.65816
.org 0x1000

start:
  ; set native mode
  clc
  xce

  ; set A to 8-bit
  sep #0x20

  ; set X/Y to 16-bit
  rep #0x10

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
  ldy #0xff
delay_outer:
  ldx #0x1000
delay_inner:
  dex
  bne delay_inner
  dey
  bne delay_outer
  rts

