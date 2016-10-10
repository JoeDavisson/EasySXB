; Software SPI - by Michael Kohn
; http://www.mikekohn.net/micro/modern_6502.php

.org 0x1000
  ;; These are some generic SPI routines.  To move to different ports or
  ;; pins, the AND/OR instructions need to be changed and the output
  ;; ports changed.

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

  ;; Set bits 0,1,2 on Port 4 (P4X) as output
  lda #0x07
  ora 0xdf24
  sta 0xdf24

  ;; P40: /CS = 1
  ;; P41: CLK = 0
  ;; P44: DATA = 0
  lda #0x01
  ora 0xdf20
  and #0xf9
  sta 0xdf20

main:
  jsr spi_enable
  lda #0x30
  jsr spi_write
  lda #0xff
  jsr spi_write
  jsr spi_disable
while_1:
  jmp while_1

;; /CS = 0
spi_enable:
  lda #0xf8
  and 0xdf20
  sta 0xdf20
  rts

;; /CS = 1
spi_disable:
  lda #0x01
  ora 0xdf20
  and #0xf9
  sta 0xdf20
  rts

;; spi_write(A)
spi_write:
  ldx #8
spi_write_next_bit:
  rol
  tay
  bcc spi_write_clock_0
  lda #0x04
  ora 0xdf20
  sta 0xdf20
  lda #0x02
  ora 0xdf20
  sta 0xdf20
  jmp spi_write_clock_off
spi_write_clock_0:
  lda #0xf8
  and 0xdf20
  sta 0xdf20
  lda #0x02
  ora 0xdf20
  sta 0xdf20
spi_write_clock_off:
  lda #0x04
  and 0xdf20
  sta 0xdf20
  tya
  dex
  bne spi_write_next_bit
  rts

data:
  db 0x30, 0xff, 0x00, 0x00
;.binfile "new.img"

