;; Tape Data Recorder
;; Copyright 2016 - Michael Kohn
;;
;; TG0 plays a tone at 4545 Hz (TG0 = 41)
;; 3579545 / 16 / 4545 = 49
;;
;; P50 is an input listening for output from an LM567 Tone Decoder
;; C1 = 0.1 uF
;; R1 = 2k ohms
;; C2 = 0.01 uF
;;
;; 1 / (1.1 * 2000 * 0.0000001) = 4545Hz
;;
;; Timer 2 is prescaled / 16
;; 3579545 / 16 = 223721
;; 223721 * 0.0045s = 1006  START
;; 223721 * 0.001s  = 223   LONG
;; 223721 * 0.0005s = 111   SHORT

;; 223721 * 0.009s = 2013  START
;; 223721 * 0.004s = 895   LONG
;; 223721 * 0.002s = 447   SHORT

.65816
.org 0x1000

ARTD0 equ 0xdf71
ARTD1 equ 0xdf73
ARTD2 equ 0xdf75
ARTD3 equ 0xdf77
ACSR0 equ 0xdf70
ACSR1 equ 0xdf72
ACSR2 equ 0xdf74
ACSR3 equ 0xdf76
UIFR equ 0xdf48
UIER equ 0xdf49
PD5 equ 0xdf21
PD6 equ 0xdf22
PD7 equ 0xdf23
PDD5 equ 0xdf25
PDD6 equ 0xdf26
BCR equ 0xdf40
TCR equ 0xdf42
TER equ 0xdf43
TIER equ 0xdf46
TIFR equ 0xdf44
T7CH equ 0xdf6f
T7CL equ 0xdf6e
T6CH equ 0xdf6d
T6CL equ 0xdf6c
T5CH equ 0xdf6b
T5CL equ 0xdf6a
T4CH equ 0xdf69
T4CL equ 0xdf68
T3CH equ 0xdf67
T3CL equ 0xdf66
T2CH equ 0xdf65
T2CL equ 0xdf64
T1CH equ 0xdf63
T1CL equ 0xdf62
T0CH equ 0xdf61
T0CL equ 0xdf60
T7LH equ 0xdf5f
T7LL equ 0xdf5e
T6LH equ 0xdf5d
T6LL equ 0xdf5c
T5LH equ 0xdf5b
T5LL equ 0xdf5a
T4LH equ 0xdf59
T4LL equ 0xdf58
T3LH equ 0xdf57
T3LL equ 0xdf56
T2LH equ 0xdf55
T2LL equ 0xdf54
T1LH equ 0xdf53
T1LL equ 0xdf52
T0LH equ 0xdf51
T0LL equ 0xdf50

GET_BYTE_FROM_PC equ 0xe033
CONTROL_TONES equ 0xe009

.define START 2013
.define LONG 895
.define SHORT 447

.macro WAIT_TIMER(a)
  lda.b #(a & 0xff)
  sta T2LL
  lda.b #(a >> 8)
  sta T2CH
  lda.b #4
  sta TIFR
.scope
wait_timer:
  lda TIFR
  bit.b #0x04
  beq wait_timer
.ends
.endm

.macro RESET_TIMER
  lda.b #0xff
  sta T2LL
  sta T2CH
.endm

.macro RESET_WATCHDOG
  lda.b #0xff
  sta T0LL
  sta T0CH
.endm

.macro COMPARE_TIMER(value, label)
  ; Set A to 16-bit
  rep #0x20

  lda T2CL
  eor #0xffff
  tax
  cmp #value
  bpl label

  ; Set A to 8-bit
  sep #0x20
.endm

.macro WHILE_MARKER_ON
.scope
while_marker_on:
  lda PD5
  bit.b #1
  beq while_marker_on
.ends
.endm

.macro WHILE_MARKER_OFF
.scope
while_marker_off:
  lda PD5
  bit.b #1
  bne while_marker_off
.ends
.endm

.macro TONE_ON
  ;ldx #49
  ldx #40  ; was working
  ;ldx #29
  ldy #0
  lda.b #1
  jsr.l CONTROL_TONES
.endm

.macro TONE_OFF
  ldx #0
  ldy #0
  lda.b #0
  jsr.l CONTROL_TONES
.endm

.macro DEBUG_LED_ON
  ;; Debug LED ON
  lda.b #0x00
  sta PD7
.endm

.macro DEBUG_LED_OFF
  ;; Debug LED OFF 
  lda.b #0xff
  sta PD7
.endm


start:
  ; Disable interrupts to protect from ROM routines running
  ;sei

  ; Set native mode
  clc
  xce

  ; Set A to 8-bit
  sep #0x20

  ; Set X/Y to 16-bit
  rep #0x10

  ;lda.b #0x89
  ;sta BCR

  ; Timer control
  lda.b #0x18
  sta TCR

  ; Enable timer 2, disable timer 0
  lda TER
  ora.b #0x04
  and.b #0xfe
  sta TER

  ; disable interrupts on port 5
  lda.b #0x00
  sta UIER

  ; disable UARTs
  ;sta ACSR0
  ;sta ACSR1
  ;sta ACSR2
  ;sta ACSR3

  ; UART control: receiver enable, 8 bit, transmit enable
  lda.b #0x25
  sta ACSR0

  ; set port 5 as input
  lda.b #2
  sta PDD5

  ; set port 5 value to 0
  lda.b #0
  sta PD5

  ; set port 6 as input
  ;lda.b #0x00
  ;sta PDD6

  ;TONE_ON

main:
  ;RESET_WATCHDOG

  ; check for byte in UART
  lda UIFR
  bit.b #0x01
  beq read_from_port_5
  jmp process_byte

read_from_port_5:
  ;jmp main

  ; read from Port 5
  lda PD5

  ; if input is 1 there is no tone yet
  ;and.b #1
  ;cmp.b #1
  bit.b #1
  bne main

  RESET_TIMER

  ; Read start header
  WHILE_MARKER_ON

  COMPARE_TIMER(START - 400, start_reading)
  ;DEBUG_LED_OFF
  jmp main

start_reading:

  ; Set A to 8-bit
  sep #0x20

  DEBUG_LED_ON

  ;; DEBUG DEBUG DEBUG DEBUG
  ;DEBUG_LED_OFF
  ;jmp main
  ;; DEBUG DEBUG DEBUG DEBUG

  ; Read byte in
  lda.b #8
  sta bit_count

read_next_bit:
  ;; DEBUG DEBUG DEBUG DEBUG
  ;jmp read_done
  ;; DEBUG DEBUG DEBUG DEBUG

  WHILE_MARKER_OFF

retry_bit:
  RESET_TIMER

  ;; Oscilloscope DEBUG
  lda.b #2
  sta PD5

  WHILE_MARKER_ON

  COMPARE_TIMER(SHORT+150, read_1)

  ;; Ignore noise
  cpx #100
  bmi retry_bit

read_0:
  ;; Shift left with a 0
  lda data_in
  clc
  rol
  sta data_in
  bra read_done

read_1:
  ; Set A to 8-bit
  sep #0x20

  ;; Shift left with a 1
  lda data_in
  sec
  rol
  ;ora.b #0x01
  sta data_in

read_done:
  ;; Oscilloscope DEBUG
  lda.b #0
  sta PD5

  lda bit_count
  dec
  sta bit_count
  bne read_next_bit

  ; Write data_in to UART
  lda data_in
  sta ARTD0

  DEBUG_LED_OFF
  jmp main

tone_is_off:
  jmp main
 
process_byte:
  DEBUG_LED_ON

  lda ARTD0
  sta data_out

  ; Echo byte on UART
  ;sta ARTD0

  ;; Clear interrupt flag
  lda UIFR
  and.b #0xfe
  sta UIFR

  ;; Send start bit
  TONE_ON
  WAIT_TIMER(START)
  TONE_OFF
  WAIT_TIMER(SHORT)

  lda.b #8
  sta bit_count
write_next_bit:
  lda data_out
  rol
  sta data_out

  bcs write_one
  TONE_ON
  WAIT_TIMER(SHORT)
  bra write_space

write_one:
  TONE_ON
  WAIT_TIMER(LONG)

write_space:
  TONE_OFF
  WAIT_TIMER(SHORT)

  lda bit_count
  dec
  sta bit_count
  beq write_finished
  jmp write_next_bit

write_finished:
  ; Send '*' to signal byte has been processed
  lda.b #'*'
  sta ARTD0

  DEBUG_LED_OFF
  jmp main

.align 16

  ;; local variables
bit_count:
  db 0, 0
data_out:
  db 0, 0
data_in:
  db 0, 0
debug:
  db 0, 0

