.65816

.org 0x1000
start:
  ; change to 16-bit mode
  clc
  xce
  ; all 16-bit registers
  rep #0x30

  sep #0x20

  ; disable interrupts on port 5
  lda.b #0
  sta 0xdf48

  ; disable UARTs
  sta 0xdf70
  sta 0xdf72
  sta 0xdf74
  sta 0xdf76

  ; set port 5 as input
  lda.b #0
  sta 0xdf25

  lda.b #0xff
  sta 0xdf21

  sep #0x20

main_while_true:
  rep #0x10

  ; ioport_getPortInputValue
  lda.b #0xff
  ;sep #0x20
  lda 0xdf21
  ;rep #0x30

  cmp 0x2000
  beq main_while_true

  sta 0x2000

  tax
  and.b #1
  bne main_not_1
  ;; LED on
  lda.b #0x00
  sta 0xdf23
  ldx #1357
  ldy #905
  lda.b #3
  jsr.l 0xe009
  jmp main_while_true
main_not_1:

  txa
  and.b #2
  bne main_not_2
  ;; LED on
  lda.b #0x00
  sta 0xdf23
  ldx #959
  ldy #678
  lda.b #3
  jsr.l 0xe009
  jmp main_while_true
main_not_2:

  txa
  and.b #4
  bne main_not_4
  ;; LED on
  lda.b #0x00
  sta 0xdf23
  ldx #1209
  ldy #959
  lda.b #3
  jsr.l 0xe009
  jmp main_while_true
main_not_4:

  txa
  and.b #8
  bne main_not_8
  ;; LED on
  lda.b #0x00
  sta 0xdf23
  ldx #1438
  ldy #1209
  lda.b #3
  jsr.l 0xe009
  jmp main_while_true
main_not_8:

  ;; LED off
  lda.b #0xff
  sta 0xdf23

  ldx #0
  ldy #0
  ;sep #0x20
  lda.b #0
  jsr.l 0xe009
  ;rep #0x30

  jmp main_while_true

