;.65816
.65xx

.org 0x1000
start:
  ;sec
  ;xce
  ;rts

main:
  ;; LED off
  ;lda #0x04
  lda #0xff
  sta 0xdf23
  jsr delay
 
  ;; LED on
  lda #0x00
  sta 0xdf23
  jsr delay

  jmp main

delay:
  ;ldy #10
delay_outer:
  ldx #0
delay_inner:
  dex
  bne delay_inner

  ;dey
  ;bne delay_outer
  rts


