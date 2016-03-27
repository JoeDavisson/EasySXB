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

  ;; Set pins 0,1,2 of Port 4 (P4X) as output
  lda #0x07
  sta 0xdf24

  ;; P40: /CS = 1
  ;; P41: CLK = 0
  ;; P44: DATA = 0
  lda #0x01
  sta 0xdf20

  ; LED on
  lda #0x00
  sta 0xdf23

main:
  ;jsr spi_enable
  lda #0x00
  sta 0xdf20

high_byte:
  lda data
  cmp #0
  beq main_sound_done

  ;; Write DAC high byte
  ldx #8
high_spi_write_next_bit:
  rol
  tay
  lda #0x02
  bcc low_clock_0
  lda #0x06
low_clock_0:
  sta 0xdf20
  lda #0x00
  sta 0xdf20
  tya
  dex
  bne high_spi_write_next_bit

  ;; Write DAC low byte
low_byte:
  lda data + 1
  ldx #8
low_spi_write_next_bit:
  rol
  tay
  lda #0x02
  bcc high_clock_0
  lda #0x06
high_clock_0:
  sta 0xdf20
  lda #0x00
  sta 0xdf20
  tya
  dex
  bne low_spi_write_next_bit

  ;jsr spi_disable
  lda #0x01
  sta 0xdf20

  ;; Increment pointer to high DAC byte
  lda #2
  clc
  adc high_byte + 1
  sta high_byte + 1
  lda #0
  adc high_byte + 2
  sta high_byte + 2

  ;; Increment pointer to low DAC byte
  lda #2
  clc
  adc low_byte + 1
  sta low_byte + 1
  lda #0
  adc low_byte + 2
  sta low_byte + 2

  jmp main

main_sound_done:
  ;; LED off
  lda #0xff
  sta 0xdf23

  ;; Reset pointer to sound
  lda #data & 0xff
  sta high_byte + 1 
  lda #data >> 8
  sta high_byte + 2 
  lda #(data + 1) & 0xff
  sta low_byte + 1 
  lda #(data + 1) >> 8
  sta low_byte + 2 

  rts
while_1:
  jmp while_1

.org 0x1200
data:
  ;db 0x3f, 0xff, 0x30, 0x00, 0x00, 0x00
;.binfile "new.img"
;.binfile "sound.img"

