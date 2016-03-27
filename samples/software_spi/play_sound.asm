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

  ;; Set entire Port 4 (P4X) as output
  lda #0xff
  sta 0xdf24

  ;; P40: /CS = 1
  ;; P41: CLK = 0
  ;; P44: DATA = 0
  lda #0x01
  sta 0xdf20

.if 0
test:
  jsr spi_enable
  lda #0x3f
  jsr spi_write
  lda #0xff
  jsr spi_write
  jsr spi_disable
while_1:
  jmp while_1
.endif

main:
  jsr spi_enable
low_byte:
  lda data
  cmp #0
  beq main_sound_done
  jsr spi_write
high_byte:
  lda data + 1
  jsr spi_write
  jsr spi_disable

  ;; Increment pointer to low DAC byte
  lda #2
  clc
  adc low_byte + 1
  sta low_byte + 1
  lda #0
  adc low_byte + 2
  sta low_byte + 2

  ;; Increment pointer to high DAC byte
  lda #2
  clc
  adc high_byte + 1
  sta high_byte + 1
  lda #0
  adc high_byte + 2
  sta high_byte + 2

  ;; Delay
  ldx #0x00
delay:
  dex
  bne delay
  jmp main

main_sound_done:
  jmp main_sound_done

;; /CS = 0
spi_enable:
  lda #0x00
  sta 0xdf20
  rts

;; /CS = 1
spi_disable:
  lda #0x01
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
  sta 0xdf20
  lda #0x06
  sta 0xdf20
  jmp spi_write_clock_off
spi_write_clock_0:
  lda #0x00
  sta 0xdf20
  lda #0x02
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

