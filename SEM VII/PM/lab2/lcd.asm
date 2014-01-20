; ******************************************************
; BASIC .ASM template file for AVR
; ******************************************************

.include "C:\PROGRA~2\VMLAB\include\m16def.inc"

; Define here the variables
;
.def  rest  =r16
.def  mod   =r17

; LCD related constants
.EQU LCD_DATA_PORT = PORTD
.EQU LCD_E_PORT    = PORTD
.EQU LCD_RS_PORT   = PORTD

.EQU LCD_DATA_DDR  = DDRD
.EQU LCD_E_DDR     = DDRD
.EQU LCD_RS_DDR    = DDRD

.EQU OE = 1
.EQU RS = 0
.EQU D4 = 2
.EQU D5 = 3
.EQU D6 = 4
.EQU D7 = 5

; Define here Reset and interrupt vectors, if any
;
reset:
   rjmp start
   reti      ; Addr $01
   reti      ; Addr $02
   reti      ; Addr $03
   reti      ; Addr $04
   reti      ; Addr $05
   reti      ; Addr $06        Use 'rjmp myVector'
   reti      ; Addr $07        to define a interrupt vector
   reti      ; Addr $08
   reti      ; Addr $09
   reti      ; Addr $0A
   reti      ; Addr $0B        This is just an example
   reti      ; Addr $0C        Not all MCUs have the same
   reti      ; Addr $0D        number of interrupt vectors
   reti      ; Addr $0E
   reti      ; Addr $0F
   reti      ; Addr $10

; Program starts here after Reset
;
start:
  ; set stack pointer
  LDI R16, LOW(RAMEND)
  OUT SPL, R16
  LDI R16, HIGH(RAMEND)
  OUT SPH, R16

  call lcd_init

  LDI R16, 0b00101000
  RCALL send_command
  RCALL wait2ms

  ; display control: on
  LDI R16, 15
  RCALL send_command
  RCALL wait2ms

  ; clear display
  LDI R16, 1
  RCALL send_command
  RCALL wait2ms

  ; return home
  LDI R16, 3
  RCALL send_command
  RCALL wait2ms

  ; entry mode set
  LDI R16, 6
  RCALL send_command
  RCALL wait2ms

  LDI R16, 254
  RCALL show


forever:
  nop
  nop       ; Infinite loop.
  nop       ; Define your main system
  nop       ; behaviour here
rjmp forever


; displays number from register R16
show:
  RCALL mod10
  PUSH R16

  MOV R16, R17

  LDI R20, 0
  CPSE R17, R20
  RCALL show

  POP R16
  SUBI R16, (-48)
  RCALL send_data
  RCALL wait2ms

  RET


; takes arg from R16 and returns
mod10:
  LDI R17, 0

  mod10loop:
    CPI R16, 10
    BRLO mod10end
    SUBI R16, 10
    INC R17
    rjmp mod10loop

mod10end:
  RET


wait2ms:
  PUSH R18
  PUSH R19

  LDI R18, 2
  LDI R19, 0
  RCALL wait

  POP R19
  POP R18
  RET


; send bits 4-7 from byte in R16
nibble:
  CBI LCD_DATA_PORT, D4
  CBI LCD_DATA_PORT, D5
  CBI LCD_DATA_PORT, D6
  CBI LCD_DATA_PORT, D7

  SBRC R16, 4
  SBI LCD_DATA_PORT, D4
  SBRC R16, 5
  SBI LCD_DATA_PORT, D5
  SBRC R16, 6
  SBI LCD_DATA_PORT, D6
  SBRC R16, 7
  SBI LCD_DATA_PORT, D7

  RET


; sends byte from R16
send_byte:
  ; first nibble
  SBI LCD_E_PORT, OE
  RCALL nibble
  nop
  nop
  nop
  CBI LCD_E_PORT, OE

  ; pause
  nop
  nop
  nop

  ; second nibble
  SWAP R16
  SBI LCD_E_PORT, OE
  RCALL nibble
  nop
  nop
  nop
  CBI LCD_E_PORT, OE

  ; wait 40 micros
  SWAP R16
  LDI R18, LOW(1)
  LDI R19, LOW(1)

  RET

; sends command from R16
send_command:
  CBI LCD_RS_PORT, RS
  RCALL send_byte
  RET

; sends data from R16
send_data:
  SBI LCD_RS_PORT, RS
  RCALL send_byte
  RET

lcd_init:
  SBI LCD_E_DDR, OE
  SBI LCD_RS_DDR, RS

  SBI LCD_DATA_DDR, D4
  SBI LCD_DATA_DDR, D5
  SBI LCD_DATA_DDR, D6
  SBI LCD_DATA_DDR, D7


  CBI LCD_RS_PORT, RS
  CBI LCD_E_PORT, OE
  ; wait 40 ms
  LDI R18, LOW(100)
  LDI R19, HIGH(100)
  rcall wait

  ; first (30) instruction
  SBI LCD_E_PORT, OE
  SBI LCD_DATA_PORT, D4
  SBI LCD_DATA_PORT, D5
  CBI LCD_DATA_PORT, D6
  CBI LCD_DATA_PORT, D7
  nop
  nop
  nop
  CBI LCD_DATA_PORT, OE
  ; wait 4 ms
  LDI R18, LOW(10)
  LDI R19, HIGH(10)
  rcall wait

  ; second (30) instruction
  SBI LCD_E_PORT, OE
  nop
  nop
  nop
  CBI LCD_DATA_PORT, OE
  ; wait 100 micros
  LDI R18, LOW(1)
  LDI R19, HIGH(1)
  rcall wait

  ; third (30) instruction
  SBI LCD_E_PORT, OE
  nop
  nop
  nop
  CBI LCD_DATA_PORT, OE
  ; wait 100 micros
  LDI R18, LOW(1)
  LDI R19, HIGH(1)
  rcall wait

  ; 4bit interface activation
  SBI LCD_E_PORT, OE
  CBI LCD_DATA_PORT, D4
  nop
  nop
  nop
  CBI LCD_E_PORT, OE
  ; wait 40 micros
  LDI R18, LOW(1)
  LDI R19, HIGH(1)
  rcall wait

  ret




; waits for (R18, R19) miliseconds
wait:
  PUSH R20
  PUSH R21
  SUBI R18, 1
  SBCI R19, 0
  BRCS wait_end

wait_main:
  LDI R20, LOW(1998)
  LDI R21, HIGH(1998)
  wait_chunk:
    SUBI R20, 1
    SBCI R21, 0
    BRCC wait_chunk
    nop
  SUBI R18, 1
  SBCI R19, 0
  BRCC wait_main

wait_end:
  POP R20
  POP R21
  RET

