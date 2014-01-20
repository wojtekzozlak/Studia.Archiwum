; ******************************************************
; BASIC .ASM template file for AVR
; ******************************************************

.include "C:\PROGRA~2\VMLAB\include\m16def.inc"

; Define here the variables
;
.def rr16 = r16
.def low = r18
.def high = r19
.def top = r20
.equ STACK = $400

.CSEG
.ORG 0
jmp start

.ORG 42
start:
  ; set stack pointer
  LDI R16, LOW(STACK)
  OUT SPL, R16
  LDI R16, HIGH(STACK)
  OUT SPH, R16

  ; initialize diode related things
  LDI R17, 255
  OUT DDRA, R17
  OUT PORTA, R17


forever:
  IN R16, PORTA
;  EOR R16, R17
  DEC R16

  OUT PORTA, R16

  LDI R18, LOW(500)
  LDI R19, HIGH(500)
  RCALL wait

  rjmp forever

; waits for (R18, R19) miliseconds
wait:
  PUSH R20
  PUSH R21
  SUBI R20, 1
  SBCI R21, 0
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
