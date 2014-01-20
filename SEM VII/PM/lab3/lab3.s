	.file	"lab3.c"
__SREG__ = 0x3f
__SP_H__ = 0x3e
__SP_L__ = 0x3d
__CCP__  = 0x34
__tmp_reg__ = 0
__zero_reg__ = 1
	.text
.global	swap
	.type	swap, @function
swap:
	push r29
	push r28
	push __tmp_reg__
	in r28,__SP_L__
	in r29,__SP_H__
/* prologue: function */
/* frame size = 1 */
	std Y+1,r24
	ldd r24,Y+1
	mov r24,r24
	ldi r25,lo8(0)
	swap r24
	swap r25
	andi r25,0xf0
	eor r25,r24
	andi r24,0xf0
	eor r25,r24
	mov r25,r24
	ldd r24,Y+1
	swap r24
	andi r24,lo8(15)
	or r24,r25
/* epilogue start */
	pop __tmp_reg__
	pop r28
	pop r29
	ret
	.size	swap, .-swap
.global	send_nibble
	.type	send_nibble, @function
send_nibble:
	push r29
	push r28
	push __tmp_reg__
	in r28,__SP_L__
	in r29,__SP_H__
/* prologue: function */
/* frame size = 1 */
	std Y+1,r24
	ldi r26,lo8(50)
	ldi r27,hi8(50)
	ldi r30,lo8(50)
	ldi r31,hi8(50)
	ld r24,Z
	andi r24,lo8(-5)
	st X,r24
	ldi r26,lo8(50)
	ldi r27,hi8(50)
	ldi r30,lo8(50)
	ldi r31,hi8(50)
	ld r24,Z
	andi r24,lo8(-9)
	st X,r24
	ldi r26,lo8(50)
	ldi r27,hi8(50)
	ldi r30,lo8(50)
	ldi r31,hi8(50)
	ld r24,Z
	andi r24,lo8(-17)
	st X,r24
	ldi r26,lo8(50)
	ldi r27,hi8(50)
	ldi r30,lo8(50)
	ldi r31,hi8(50)
	ld r24,Z
	andi r24,lo8(-33)
	st X,r24
	ldd r24,Y+1
	mov r24,r24
	ldi r25,lo8(0)
	andi r24,lo8(16)
	andi r25,hi8(16)
	sbiw r24,0
	breq .L4
	ldi r26,lo8(50)
	ldi r27,hi8(50)
	ldi r30,lo8(50)
	ldi r31,hi8(50)
	ld r24,Z
	ori r24,lo8(4)
	st X,r24
.L4:
	ldd r24,Y+1
	mov r24,r24
	ldi r25,lo8(0)
	andi r24,lo8(32)
	andi r25,hi8(32)
	sbiw r24,0
	breq .L5
	ldi r26,lo8(50)
	ldi r27,hi8(50)
	ldi r30,lo8(50)
	ldi r31,hi8(50)
	ld r24,Z
	ori r24,lo8(8)
	st X,r24
.L5:
	ldd r24,Y+1
	mov r24,r24
	ldi r25,lo8(0)
	andi r24,lo8(64)
	andi r25,hi8(64)
	sbiw r24,0
	breq .L6
	ldi r26,lo8(50)
	ldi r27,hi8(50)
	ldi r30,lo8(50)
	ldi r31,hi8(50)
	ld r24,Z
	ori r24,lo8(16)
	st X,r24
.L6:
	ldd r24,Y+1
	tst r24
	brge .L8
	ldi r26,lo8(50)
	ldi r27,hi8(50)
	ldi r30,lo8(50)
	ldi r31,hi8(50)
	ld r24,Z
	ori r24,lo8(32)
	st X,r24
.L8:
/* epilogue start */
	pop __tmp_reg__
	pop r28
	pop r29
	ret
	.size	send_nibble, .-send_nibble
.global	send_byte
	.type	send_byte, @function
send_byte:
	push r29
	push r28
	in r28,__SP_L__
	in r29,__SP_H__
	sbiw r28,15
	in __tmp_reg__,__SREG__
	cli
	out __SP_H__,r29
	out __SREG__,__tmp_reg__
	out __SP_L__,r28
/* prologue: function */
/* frame size = 15 */
	std Y+15,r24
	ldi r26,lo8(50)
	ldi r27,hi8(50)
	ldi r30,lo8(50)
	ldi r31,hi8(50)
	ld r24,Z
	ori r24,lo8(2)
	st X,r24
	ldd r24,Y+15
	call send_nibble
/* #APP */
 ;  51 "z:\PM\lab3\lab3.c" 1
	nop;nop;nop;
 ;  0 "" 2
/* #NOAPP */
	ldi r26,lo8(50)
	ldi r27,hi8(50)
	ldi r30,lo8(50)
	ldi r31,hi8(50)
	ld r24,Z
	andi r24,lo8(-3)
	st X,r24
	ldd r24,Y+15
	call swap
	std Y+15,r24
	ldi r26,lo8(50)
	ldi r27,hi8(50)
	ldi r30,lo8(50)
	ldi r31,hi8(50)
	ld r24,Z
	andi r24,lo8(-3)
	st X,r24
	ldd r24,Y+15
	call send_nibble
/* #APP */
 ;  57 "z:\PM\lab3\lab3.c" 1
	nop;nop;nop;
 ;  0 "" 2
/* #NOAPP */
	ldi r26,lo8(50)
	ldi r27,hi8(50)
	ldi r30,lo8(50)
	ldi r31,hi8(50)
	ld r24,Z
	andi r24,lo8(-3)
	st X,r24
	ldi r24,lo8(0x42200000)
	ldi r25,hi8(0x42200000)
	ldi r26,hlo8(0x42200000)
	ldi r27,hhi8(0x42200000)
	std Y+11,r24
	std Y+12,r25
	std Y+13,r26
	std Y+14,r27
	ldd r22,Y+11
	ldd r23,Y+12
	ldd r24,Y+13
	ldd r25,Y+14
	ldi r18,lo8(0x44fa0000)
	ldi r19,hi8(0x44fa0000)
	ldi r20,hlo8(0x44fa0000)
	ldi r21,hhi8(0x44fa0000)
	call __mulsf3
	movw r26,r24
	movw r24,r22
	std Y+7,r24
	std Y+8,r25
	std Y+9,r26
	std Y+10,r27
	ldd r22,Y+7
	ldd r23,Y+8
	ldd r24,Y+9
	ldd r25,Y+10
	ldi r18,lo8(0x3f800000)
	ldi r19,hi8(0x3f800000)
	ldi r20,hlo8(0x3f800000)
	ldi r21,hhi8(0x3f800000)
	call __ltsf2
	tst r24
	brge .L21
.L19:
	ldi r24,lo8(1)
	ldi r25,hi8(1)
	std Y+6,r25
	std Y+5,r24
	rjmp .L12
.L21:
	ldd r22,Y+7
	ldd r23,Y+8
	ldd r24,Y+9
	ldd r25,Y+10
	ldi r18,lo8(0x477fff00)
	ldi r19,hi8(0x477fff00)
	ldi r20,hlo8(0x477fff00)
	ldi r21,hhi8(0x477fff00)
	call __gtsf2
	cp __zero_reg__,r24
	brge .L22
.L20:
	ldd r22,Y+11
	ldd r23,Y+12
	ldd r24,Y+13
	ldd r25,Y+14
	ldi r18,lo8(0x41200000)
	ldi r19,hi8(0x41200000)
	ldi r20,hlo8(0x41200000)
	ldi r21,hhi8(0x41200000)
	call __mulsf3
	movw r26,r24
	movw r24,r22
	movw r22,r24
	movw r24,r26
	call __fixunssfsi
	movw r26,r24
	movw r24,r22
	std Y+6,r25
	std Y+5,r24
	rjmp .L15
.L16:
	ldi r24,lo8(200)
	ldi r25,hi8(200)
	std Y+4,r25
	std Y+3,r24
	ldd r24,Y+3
	ldd r25,Y+4
/* #APP */
 ;  105 "c:/program files/winavr-20100110/lib/gcc/../../avr/include/util/delay_basic.h" 1
	1: sbiw r24,1
	brne 1b
 ;  0 "" 2
/* #NOAPP */
	std Y+4,r25
	std Y+3,r24
	ldd r24,Y+5
	ldd r25,Y+6
	sbiw r24,1
	std Y+6,r25
	std Y+5,r24
.L15:
	ldd r24,Y+5
	ldd r25,Y+6
	sbiw r24,0
	brne .L16
	rjmp .L18
.L22:
	ldd r22,Y+7
	ldd r23,Y+8
	ldd r24,Y+9
	ldd r25,Y+10
	call __fixunssfsi
	movw r26,r24
	movw r24,r22
	std Y+6,r25
	std Y+5,r24
.L12:
	ldd r24,Y+5
	ldd r25,Y+6
	std Y+2,r25
	std Y+1,r24
	ldd r24,Y+1
	ldd r25,Y+2
/* #APP */
 ;  105 "c:/program files/winavr-20100110/lib/gcc/../../avr/include/util/delay_basic.h" 1
	1: sbiw r24,1
	brne 1b
 ;  0 "" 2
/* #NOAPP */
	std Y+2,r25
	std Y+1,r24
.L18:
/* epilogue start */
	adiw r28,15
	in __tmp_reg__,__SREG__
	cli
	out __SP_H__,r29
	out __SREG__,__tmp_reg__
	out __SP_L__,r28
	pop r28
	pop r29
	ret
	.size	send_byte, .-send_byte
.global	send_command
	.type	send_command, @function
send_command:
	push r29
	push r28
	push __tmp_reg__
	in r28,__SP_L__
	in r29,__SP_H__
/* prologue: function */
/* frame size = 1 */
	std Y+1,r24
	ldi r26,lo8(50)
	ldi r27,hi8(50)
	ldi r30,lo8(50)
	ldi r31,hi8(50)
	ld r24,Z
	andi r24,lo8(-2)
	st X,r24
	ldd r24,Y+1
	call send_byte
/* epilogue start */
	pop __tmp_reg__
	pop r28
	pop r29
	ret
	.size	send_command, .-send_command
.global	send_data
	.type	send_data, @function
send_data:
	push r29
	push r28
	push __tmp_reg__
	in r28,__SP_L__
	in r29,__SP_H__
/* prologue: function */
/* frame size = 1 */
	std Y+1,r24
	ldi r26,lo8(50)
	ldi r27,hi8(50)
	ldi r30,lo8(50)
	ldi r31,hi8(50)
	ld r24,Z
	ori r24,lo8(1)
	st X,r24
	ldd r24,Y+1
	call send_byte
/* epilogue start */
	pop __tmp_reg__
	pop r28
	pop r29
	ret
	.size	send_data, .-send_data
.global	lcd_init
	.type	lcd_init, @function
lcd_init:
	push r16
	push r17
	push r29
	push r28
	in r28,__SP_L__
	in r29,__SP_H__
	subi r28,lo8(-(-100))
	sbci r29,hi8(-(-100))
	in __tmp_reg__,__SREG__
	cli
	out __SP_H__,r29
	out __SREG__,__tmp_reg__
	out __SP_L__,r28
/* prologue: function */
/* frame size = 100 */
	ldi r26,lo8(49)
	ldi r27,hi8(49)
	ldi r30,lo8(49)
	ldi r31,hi8(49)
	ld r24,Z
	ori r24,lo8(1)
	st X,r24
	ldi r26,lo8(49)
	ldi r27,hi8(49)
	ldi r30,lo8(49)
	ldi r31,hi8(49)
	ld r24,Z
	ori r24,lo8(2)
	st X,r24
	ldi r26,lo8(49)
	ldi r27,hi8(49)
	ldi r30,lo8(49)
	ldi r31,hi8(49)
	ld r24,Z
	ori r24,lo8(4)
	st X,r24
	ldi r26,lo8(49)
	ldi r27,hi8(49)
	ldi r30,lo8(49)
	ldi r31,hi8(49)
	ld r24,Z
	ori r24,lo8(8)
	st X,r24
	ldi r26,lo8(49)
	ldi r27,hi8(49)
	ldi r30,lo8(49)
	ldi r31,hi8(49)
	ld r24,Z
	ori r24,lo8(16)
	st X,r24
	ldi r26,lo8(49)
	ldi r27,hi8(49)
	ldi r30,lo8(49)
	ldi r31,hi8(49)
	ld r24,Z
	ori r24,lo8(32)
	st X,r24
	ldi r26,lo8(50)
	ldi r27,hi8(50)
	ldi r30,lo8(50)
	ldi r31,hi8(50)
	ld r24,Z
	andi r24,lo8(-2)
	st X,r24
	ldi r26,lo8(50)
	ldi r27,hi8(50)
	ldi r30,lo8(50)
	ldi r31,hi8(50)
	ld r24,Z
	andi r24,lo8(-3)
	st X,r24
	movw r30,r28
	subi r30,lo8(-(97))
	sbci r31,hi8(-(97))
	ldi r24,lo8(0x42200000)
	ldi r25,hi8(0x42200000)
	ldi r26,hlo8(0x42200000)
	ldi r27,hhi8(0x42200000)
	st Z,r24
	std Z+1,r25
	std Z+2,r26
	std Z+3,r27
	movw r16,r28
	subi r16,lo8(-(93))
	sbci r17,hi8(-(93))
	movw r30,r28
	subi r30,lo8(-(97))
	sbci r31,hi8(-(97))
	ld r22,Z
	ldd r23,Z+1
	ldd r24,Z+2
	ldd r25,Z+3
	ldi r18,lo8(0x44fa0000)
	ldi r19,hi8(0x44fa0000)
	ldi r20,hlo8(0x44fa0000)
	ldi r21,hhi8(0x44fa0000)
	call __mulsf3
	movw r26,r24
	movw r24,r22
	movw r30,r16
	st Z,r24
	std Z+1,r25
	std Z+2,r26
	std Z+3,r27
	movw r30,r28
	subi r30,lo8(-(93))
	sbci r31,hi8(-(93))
	ld r22,Z
	ldd r23,Z+1
	ldd r24,Z+2
	ldd r25,Z+3
	ldi r18,lo8(0x3f800000)
	ldi r19,hi8(0x3f800000)
	ldi r20,hlo8(0x3f800000)
	ldi r21,hhi8(0x3f800000)
	call __ltsf2
	tst r24
	brge .L103
.L87:
	movw r30,r28
	subi r30,lo8(-(91))
	sbci r31,hi8(-(91))
	ldi r24,lo8(1)
	ldi r25,hi8(1)
	std Z+1,r25
	st Z,r24
	rjmp .L30
.L103:
	movw r30,r28
	subi r30,lo8(-(93))
	sbci r31,hi8(-(93))
	ld r22,Z
	ldd r23,Z+1
	ldd r24,Z+2
	ldd r25,Z+3
	ldi r18,lo8(0x477fff00)
	ldi r19,hi8(0x477fff00)
	ldi r20,hlo8(0x477fff00)
	ldi r21,hhi8(0x477fff00)
	call __gtsf2
	cp __zero_reg__,r24
	brlt .+2
	rjmp .L104
.L88:
	movw r30,r28
	subi r30,lo8(-(97))
	sbci r31,hi8(-(97))
	ld r22,Z
	ldd r23,Z+1
	ldd r24,Z+2
	ldd r25,Z+3
	ldi r18,lo8(0x41200000)
	ldi r19,hi8(0x41200000)
	ldi r20,hlo8(0x41200000)
	ldi r21,hhi8(0x41200000)
	call __mulsf3
	movw r26,r24
	movw r24,r22
	movw r16,r28
	subi r16,lo8(-(91))
	sbci r17,hi8(-(91))
	movw r22,r24
	movw r24,r26
	call __fixunssfsi
	movw r26,r24
	movw r24,r22
	movw r30,r16
	std Z+1,r25
	st Z,r24
	rjmp .L33
.L34:
	movw r30,r28
	subi r30,lo8(-(89))
	sbci r31,hi8(-(89))
	ldi r24,lo8(200)
	ldi r25,hi8(200)
	std Z+1,r25
	st Z,r24
	movw r30,r28
	subi r30,lo8(-(89))
	sbci r31,hi8(-(89))
	ld r24,Z
	ldd r25,Z+1
/* #APP */
 ;  105 "c:/program files/winavr-20100110/lib/gcc/../../avr/include/util/delay_basic.h" 1
	1: sbiw r24,1
	brne 1b
 ;  0 "" 2
/* #NOAPP */
	movw r30,r28
	subi r30,lo8(-(89))
	sbci r31,hi8(-(89))
	std Z+1,r25
	st Z,r24
	movw r26,r28
	subi r26,lo8(-(91))
	sbci r27,hi8(-(91))
	movw r30,r28
	subi r30,lo8(-(91))
	sbci r31,hi8(-(91))
	ld r24,Z
	ldd r25,Z+1
	sbiw r24,1
	adiw r26,1
	st X,r25
	st -X,r24
.L33:
	movw r30,r28
	subi r30,lo8(-(91))
	sbci r31,hi8(-(91))
	ld r24,Z
	ldd r25,Z+1
	sbiw r24,0
	brne .L34
	rjmp .L35
.L104:
	movw r16,r28
	subi r16,lo8(-(91))
	sbci r17,hi8(-(91))
	movw r30,r28
	subi r30,lo8(-(93))
	sbci r31,hi8(-(93))
	ld r22,Z
	ldd r23,Z+1
	ldd r24,Z+2
	ldd r25,Z+3
	call __fixunssfsi
	movw r26,r24
	movw r24,r22
	movw r30,r16
	std Z+1,r25
	st Z,r24
.L30:
	movw r26,r28
	subi r26,lo8(-(87))
	sbci r27,hi8(-(87))
	movw r30,r28
	subi r30,lo8(-(91))
	sbci r31,hi8(-(91))
	ld r24,Z
	ldd r25,Z+1
	st X+,r24
	st X,r25
	movw r30,r28
	subi r30,lo8(-(87))
	sbci r31,hi8(-(87))
	ld r24,Z
	ldd r25,Z+1
/* #APP */
 ;  105 "c:/program files/winavr-20100110/lib/gcc/../../avr/include/util/delay_basic.h" 1
	1: sbiw r24,1
	brne 1b
 ;  0 "" 2
/* #NOAPP */
	movw r30,r28
	subi r30,lo8(-(87))
	sbci r31,hi8(-(87))
	std Z+1,r25
	st Z,r24
.L35:
	ldi r26,lo8(50)
	ldi r27,hi8(50)
	ldi r30,lo8(50)
	ldi r31,hi8(50)
	ld r24,Z
	ori r24,lo8(2)
	st X,r24
	ldi r26,lo8(50)
	ldi r27,hi8(50)
	ldi r30,lo8(50)
	ldi r31,hi8(50)
	ld r24,Z
	ori r24,lo8(4)
	st X,r24
	ldi r26,lo8(50)
	ldi r27,hi8(50)
	ldi r30,lo8(50)
	ldi r31,hi8(50)
	ld r24,Z
	ori r24,lo8(8)
	st X,r24
	ldi r26,lo8(50)
	ldi r27,hi8(50)
	ldi r30,lo8(50)
	ldi r31,hi8(50)
	ld r24,Z
	andi r24,lo8(-17)
	st X,r24
	ldi r26,lo8(50)
	ldi r27,hi8(50)
	ldi r30,lo8(50)
	ldi r31,hi8(50)
	ld r24,Z
	andi r24,lo8(-33)
	st X,r24
/* #APP */
 ;  93 "z:\PM\lab3\lab3.c" 1
	nop;nop;nop;
 ;  0 "" 2
/* #NOAPP */
	ldi r26,lo8(50)
	ldi r27,hi8(50)
	ldi r30,lo8(50)
	ldi r31,hi8(50)
	ld r24,Z
	andi r24,lo8(-3)
	st X,r24
	movw r30,r28
	subi r30,lo8(-(83))
	sbci r31,hi8(-(83))
	ldi r24,lo8(0x40a00000)
	ldi r25,hi8(0x40a00000)
	ldi r26,hlo8(0x40a00000)
	ldi r27,hhi8(0x40a00000)
	st Z,r24
	std Z+1,r25
	std Z+2,r26
	std Z+3,r27
	movw r16,r28
	subi r16,lo8(-(79))
	sbci r17,hi8(-(79))
	movw r30,r28
	subi r30,lo8(-(83))
	sbci r31,hi8(-(83))
	ld r22,Z
	ldd r23,Z+1
	ldd r24,Z+2
	ldd r25,Z+3
	ldi r18,lo8(0x44fa0000)
	ldi r19,hi8(0x44fa0000)
	ldi r20,hlo8(0x44fa0000)
	ldi r21,hhi8(0x44fa0000)
	call __mulsf3
	movw r26,r24
	movw r24,r22
	movw r30,r16
	st Z,r24
	std Z+1,r25
	std Z+2,r26
	std Z+3,r27
	movw r30,r28
	subi r30,lo8(-(79))
	sbci r31,hi8(-(79))
	ld r22,Z
	ldd r23,Z+1
	ldd r24,Z+2
	ldd r25,Z+3
	ldi r18,lo8(0x3f800000)
	ldi r19,hi8(0x3f800000)
	ldi r20,hlo8(0x3f800000)
	ldi r21,hhi8(0x3f800000)
	call __ltsf2
	tst r24
	brge .L105
.L89:
	movw r30,r28
	subi r30,lo8(-(77))
	sbci r31,hi8(-(77))
	ldi r24,lo8(1)
	ldi r25,hi8(1)
	std Z+1,r25
	st Z,r24
	rjmp .L38
.L105:
	movw r30,r28
	subi r30,lo8(-(79))
	sbci r31,hi8(-(79))
	ld r22,Z
	ldd r23,Z+1
	ldd r24,Z+2
	ldd r25,Z+3
	ldi r18,lo8(0x477fff00)
	ldi r19,hi8(0x477fff00)
	ldi r20,hlo8(0x477fff00)
	ldi r21,hhi8(0x477fff00)
	call __gtsf2
	cp __zero_reg__,r24
	brlt .+2
	rjmp .L106
.L90:
	movw r30,r28
	subi r30,lo8(-(83))
	sbci r31,hi8(-(83))
	ld r22,Z
	ldd r23,Z+1
	ldd r24,Z+2
	ldd r25,Z+3
	ldi r18,lo8(0x41200000)
	ldi r19,hi8(0x41200000)
	ldi r20,hlo8(0x41200000)
	ldi r21,hhi8(0x41200000)
	call __mulsf3
	movw r26,r24
	movw r24,r22
	movw r16,r28
	subi r16,lo8(-(77))
	sbci r17,hi8(-(77))
	movw r22,r24
	movw r24,r26
	call __fixunssfsi
	movw r26,r24
	movw r24,r22
	movw r30,r16
	std Z+1,r25
	st Z,r24
	rjmp .L41
.L42:
	movw r30,r28
	subi r30,lo8(-(75))
	sbci r31,hi8(-(75))
	ldi r24,lo8(200)
	ldi r25,hi8(200)
	std Z+1,r25
	st Z,r24
	movw r30,r28
	subi r30,lo8(-(75))
	sbci r31,hi8(-(75))
	ld r24,Z
	ldd r25,Z+1
/* #APP */
 ;  105 "c:/program files/winavr-20100110/lib/gcc/../../avr/include/util/delay_basic.h" 1
	1: sbiw r24,1
	brne 1b
 ;  0 "" 2
/* #NOAPP */
	movw r30,r28
	subi r30,lo8(-(75))
	sbci r31,hi8(-(75))
	std Z+1,r25
	st Z,r24
	movw r26,r28
	subi r26,lo8(-(77))
	sbci r27,hi8(-(77))
	movw r30,r28
	subi r30,lo8(-(77))
	sbci r31,hi8(-(77))
	ld r24,Z
	ldd r25,Z+1
	sbiw r24,1
	adiw r26,1
	st X,r25
	st -X,r24
.L41:
	movw r30,r28
	subi r30,lo8(-(77))
	sbci r31,hi8(-(77))
	ld r24,Z
	ldd r25,Z+1
	sbiw r24,0
	brne .L42
	rjmp .L43
.L106:
	movw r16,r28
	subi r16,lo8(-(77))
	sbci r17,hi8(-(77))
	movw r30,r28
	subi r30,lo8(-(79))
	sbci r31,hi8(-(79))
	ld r22,Z
	ldd r23,Z+1
	ldd r24,Z+2
	ldd r25,Z+3
	call __fixunssfsi
	movw r26,r24
	movw r24,r22
	movw r30,r16
	std Z+1,r25
	st Z,r24
.L38:
	movw r26,r28
	subi r26,lo8(-(73))
	sbci r27,hi8(-(73))
	movw r30,r28
	subi r30,lo8(-(77))
	sbci r31,hi8(-(77))
	ld r24,Z
	ldd r25,Z+1
	st X+,r24
	st X,r25
	movw r30,r28
	subi r30,lo8(-(73))
	sbci r31,hi8(-(73))
	ld r24,Z
	ldd r25,Z+1
/* #APP */
 ;  105 "c:/program files/winavr-20100110/lib/gcc/../../avr/include/util/delay_basic.h" 1
	1: sbiw r24,1
	brne 1b
 ;  0 "" 2
/* #NOAPP */
	movw r30,r28
	subi r30,lo8(-(73))
	sbci r31,hi8(-(73))
	std Z+1,r25
	st Z,r24
.L43:
	ldi r26,lo8(50)
	ldi r27,hi8(50)
	ldi r30,lo8(50)
	ldi r31,hi8(50)
	ld r24,Z
	ori r24,lo8(2)
	st X,r24
/* #APP */
 ;  99 "z:\PM\lab3\lab3.c" 1
	nop;nop;nop;
 ;  0 "" 2
/* #NOAPP */
	ldi r26,lo8(50)
	ldi r27,hi8(50)
	ldi r30,lo8(50)
	ldi r31,hi8(50)
	ld r24,Z
	andi r24,lo8(-3)
	st X,r24
	movw r30,r28
	subi r30,lo8(-(69))
	sbci r31,hi8(-(69))
	ldi r24,lo8(0x42c80000)
	ldi r25,hi8(0x42c80000)
	ldi r26,hlo8(0x42c80000)
	ldi r27,hhi8(0x42c80000)
	st Z,r24
	std Z+1,r25
	std Z+2,r26
	std Z+3,r27
	movw r16,r28
	subi r16,lo8(-(65))
	sbci r17,hi8(-(65))
	movw r30,r28
	subi r30,lo8(-(69))
	sbci r31,hi8(-(69))
	ld r22,Z
	ldd r23,Z+1
	ldd r24,Z+2
	ldd r25,Z+3
	ldi r18,lo8(0x402aaaab)
	ldi r19,hi8(0x402aaaab)
	ldi r20,hlo8(0x402aaaab)
	ldi r21,hhi8(0x402aaaab)
	call __mulsf3
	movw r26,r24
	movw r24,r22
	movw r30,r16
	st Z,r24
	std Z+1,r25
	std Z+2,r26
	std Z+3,r27
	movw r30,r28
	subi r30,lo8(-(65))
	sbci r31,hi8(-(65))
	ld r22,Z
	ldd r23,Z+1
	ldd r24,Z+2
	ldd r25,Z+3
	ldi r18,lo8(0x3f800000)
	ldi r19,hi8(0x3f800000)
	ldi r20,hlo8(0x3f800000)
	ldi r21,hhi8(0x3f800000)
	call __ltsf2
	tst r24
	brge .L107
.L91:
	ldi r24,lo8(1)
	movw r30,r28
	subi r30,lo8(-(64))
	sbci r31,hi8(-(64))
	st Z,r24
	rjmp .L46
.L107:
	movw r30,r28
	subi r30,lo8(-(65))
	sbci r31,hi8(-(65))
	ld r22,Z
	ldd r23,Z+1
	ldd r24,Z+2
	ldd r25,Z+3
	ldi r18,lo8(0x437f0000)
	ldi r19,hi8(0x437f0000)
	ldi r20,hlo8(0x437f0000)
	ldi r21,hhi8(0x437f0000)
	call __gtsf2
	cp __zero_reg__,r24
	brlt .+2
	rjmp .L108
.L92:
	movw r30,r28
	subi r30,lo8(-(69))
	sbci r31,hi8(-(69))
	ld r22,Z
	ldd r23,Z+1
	ldd r24,Z+2
	ldd r25,Z+3
	ldi r18,lo8(0x447a0000)
	ldi r19,hi8(0x447a0000)
	ldi r20,hlo8(0x447a0000)
	ldi r21,hhi8(0x447a0000)
	call __divsf3
	movw r26,r24
	movw r24,r22
	std Y+60,r24
	std Y+61,r25
	std Y+62,r26
	std Y+63,r27
	ldd r22,Y+60
	ldd r23,Y+61
	ldd r24,Y+62
	ldd r25,Y+63
	ldi r18,lo8(0x44fa0000)
	ldi r19,hi8(0x44fa0000)
	ldi r20,hlo8(0x44fa0000)
	ldi r21,hhi8(0x44fa0000)
	call __mulsf3
	movw r26,r24
	movw r24,r22
	std Y+56,r24
	std Y+57,r25
	std Y+58,r26
	std Y+59,r27
	ldd r22,Y+56
	ldd r23,Y+57
	ldd r24,Y+58
	ldd r25,Y+59
	ldi r18,lo8(0x3f800000)
	ldi r19,hi8(0x3f800000)
	ldi r20,hlo8(0x3f800000)
	ldi r21,hhi8(0x3f800000)
	call __ltsf2
	tst r24
	brge .L109
.L93:
	ldi r24,lo8(1)
	ldi r25,hi8(1)
	std Y+55,r25
	std Y+54,r24
	rjmp .L51
.L109:
	ldd r22,Y+56
	ldd r23,Y+57
	ldd r24,Y+58
	ldd r25,Y+59
	ldi r18,lo8(0x477fff00)
	ldi r19,hi8(0x477fff00)
	ldi r20,hlo8(0x477fff00)
	ldi r21,hhi8(0x477fff00)
	call __gtsf2
	cp __zero_reg__,r24
	brge .L110
.L94:
	ldd r22,Y+60
	ldd r23,Y+61
	ldd r24,Y+62
	ldd r25,Y+63
	ldi r18,lo8(0x41200000)
	ldi r19,hi8(0x41200000)
	ldi r20,hlo8(0x41200000)
	ldi r21,hhi8(0x41200000)
	call __mulsf3
	movw r26,r24
	movw r24,r22
	movw r22,r24
	movw r24,r26
	call __fixunssfsi
	movw r26,r24
	movw r24,r22
	std Y+55,r25
	std Y+54,r24
	rjmp .L54
.L55:
	ldi r24,lo8(200)
	ldi r25,hi8(200)
	std Y+53,r25
	std Y+52,r24
	ldd r24,Y+52
	ldd r25,Y+53
/* #APP */
 ;  105 "c:/program files/winavr-20100110/lib/gcc/../../avr/include/util/delay_basic.h" 1
	1: sbiw r24,1
	brne 1b
 ;  0 "" 2
/* #NOAPP */
	std Y+53,r25
	std Y+52,r24
	ldd r24,Y+54
	ldd r25,Y+55
	sbiw r24,1
	std Y+55,r25
	std Y+54,r24
.L54:
	ldd r24,Y+54
	ldd r25,Y+55
	sbiw r24,0
	brne .L55
	rjmp .L57
.L110:
	ldd r22,Y+56
	ldd r23,Y+57
	ldd r24,Y+58
	ldd r25,Y+59
	call __fixunssfsi
	movw r26,r24
	movw r24,r22
	std Y+55,r25
	std Y+54,r24
.L51:
	ldd r24,Y+54
	ldd r25,Y+55
	std Y+51,r25
	std Y+50,r24
	ldd r24,Y+50
	ldd r25,Y+51
/* #APP */
 ;  105 "c:/program files/winavr-20100110/lib/gcc/../../avr/include/util/delay_basic.h" 1
	1: sbiw r24,1
	brne 1b
 ;  0 "" 2
/* #NOAPP */
	std Y+51,r25
	std Y+50,r24
	rjmp .L57
.L108:
	movw r30,r28
	subi r30,lo8(-(65))
	sbci r31,hi8(-(65))
	ld r22,Z
	ldd r23,Z+1
	ldd r24,Z+2
	ldd r25,Z+3
	call __fixunssfsi
	movw r26,r24
	movw r24,r22
	movw r30,r28
	subi r30,lo8(-(64))
	sbci r31,hi8(-(64))
	st Z,r24
.L46:
	movw r30,r28
	subi r30,lo8(-(64))
	sbci r31,hi8(-(64))
	ld r24,Z
	std Y+49,r24
	ldd r24,Y+49
/* #APP */
 ;  83 "c:/program files/winavr-20100110/lib/gcc/../../avr/include/util/delay_basic.h" 1
	1: dec r24
	brne 1b
 ;  0 "" 2
/* #NOAPP */
	std Y+49,r24
.L57:
	ldi r26,lo8(50)
	ldi r27,hi8(50)
	ldi r30,lo8(50)
	ldi r31,hi8(50)
	ld r24,Z
	ori r24,lo8(2)
	st X,r24
/* #APP */
 ;  105 "z:\PM\lab3\lab3.c" 1
	nop;nop;nop;
 ;  0 "" 2
/* #NOAPP */
	ldi r26,lo8(50)
	ldi r27,hi8(50)
	ldi r30,lo8(50)
	ldi r31,hi8(50)
	ld r24,Z
	andi r24,lo8(-3)
	st X,r24
	ldi r24,lo8(0x42c80000)
	ldi r25,hi8(0x42c80000)
	ldi r26,hlo8(0x42c80000)
	ldi r27,hhi8(0x42c80000)
	std Y+45,r24
	std Y+46,r25
	std Y+47,r26
	std Y+48,r27
	ldd r22,Y+45
	ldd r23,Y+46
	ldd r24,Y+47
	ldd r25,Y+48
	ldi r18,lo8(0x402aaaab)
	ldi r19,hi8(0x402aaaab)
	ldi r20,hlo8(0x402aaaab)
	ldi r21,hhi8(0x402aaaab)
	call __mulsf3
	movw r26,r24
	movw r24,r22
	std Y+41,r24
	std Y+42,r25
	std Y+43,r26
	std Y+44,r27
	ldd r22,Y+41
	ldd r23,Y+42
	ldd r24,Y+43
	ldd r25,Y+44
	ldi r18,lo8(0x3f800000)
	ldi r19,hi8(0x3f800000)
	ldi r20,hlo8(0x3f800000)
	ldi r21,hhi8(0x3f800000)
	call __ltsf2
	tst r24
	brge .L111
.L95:
	ldi r24,lo8(1)
	std Y+40,r24
	rjmp .L60
.L111:
	ldd r22,Y+41
	ldd r23,Y+42
	ldd r24,Y+43
	ldd r25,Y+44
	ldi r18,lo8(0x437f0000)
	ldi r19,hi8(0x437f0000)
	ldi r20,hlo8(0x437f0000)
	ldi r21,hhi8(0x437f0000)
	call __gtsf2
	cp __zero_reg__,r24
	brlt .+2
	rjmp .L112
.L96:
	ldd r22,Y+45
	ldd r23,Y+46
	ldd r24,Y+47
	ldd r25,Y+48
	ldi r18,lo8(0x447a0000)
	ldi r19,hi8(0x447a0000)
	ldi r20,hlo8(0x447a0000)
	ldi r21,hhi8(0x447a0000)
	call __divsf3
	movw r26,r24
	movw r24,r22
	std Y+36,r24
	std Y+37,r25
	std Y+38,r26
	std Y+39,r27
	ldd r22,Y+36
	ldd r23,Y+37
	ldd r24,Y+38
	ldd r25,Y+39
	ldi r18,lo8(0x44fa0000)
	ldi r19,hi8(0x44fa0000)
	ldi r20,hlo8(0x44fa0000)
	ldi r21,hhi8(0x44fa0000)
	call __mulsf3
	movw r26,r24
	movw r24,r22
	std Y+32,r24
	std Y+33,r25
	std Y+34,r26
	std Y+35,r27
	ldd r22,Y+32
	ldd r23,Y+33
	ldd r24,Y+34
	ldd r25,Y+35
	ldi r18,lo8(0x3f800000)
	ldi r19,hi8(0x3f800000)
	ldi r20,hlo8(0x3f800000)
	ldi r21,hhi8(0x3f800000)
	call __ltsf2
	tst r24
	brge .L113
.L97:
	ldi r24,lo8(1)
	ldi r25,hi8(1)
	std Y+31,r25
	std Y+30,r24
	rjmp .L65
.L113:
	ldd r22,Y+32
	ldd r23,Y+33
	ldd r24,Y+34
	ldd r25,Y+35
	ldi r18,lo8(0x477fff00)
	ldi r19,hi8(0x477fff00)
	ldi r20,hlo8(0x477fff00)
	ldi r21,hhi8(0x477fff00)
	call __gtsf2
	cp __zero_reg__,r24
	brge .L114
.L98:
	ldd r22,Y+36
	ldd r23,Y+37
	ldd r24,Y+38
	ldd r25,Y+39
	ldi r18,lo8(0x41200000)
	ldi r19,hi8(0x41200000)
	ldi r20,hlo8(0x41200000)
	ldi r21,hhi8(0x41200000)
	call __mulsf3
	movw r26,r24
	movw r24,r22
	movw r22,r24
	movw r24,r26
	call __fixunssfsi
	movw r26,r24
	movw r24,r22
	std Y+31,r25
	std Y+30,r24
	rjmp .L68
.L69:
	ldi r24,lo8(200)
	ldi r25,hi8(200)
	std Y+29,r25
	std Y+28,r24
	ldd r24,Y+28
	ldd r25,Y+29
/* #APP */
 ;  105 "c:/program files/winavr-20100110/lib/gcc/../../avr/include/util/delay_basic.h" 1
	1: sbiw r24,1
	brne 1b
 ;  0 "" 2
/* #NOAPP */
	std Y+29,r25
	std Y+28,r24
	ldd r24,Y+30
	ldd r25,Y+31
	sbiw r24,1
	std Y+31,r25
	std Y+30,r24
.L68:
	ldd r24,Y+30
	ldd r25,Y+31
	sbiw r24,0
	brne .L69
	rjmp .L71
.L114:
	ldd r22,Y+32
	ldd r23,Y+33
	ldd r24,Y+34
	ldd r25,Y+35
	call __fixunssfsi
	movw r26,r24
	movw r24,r22
	std Y+31,r25
	std Y+30,r24
.L65:
	ldd r24,Y+30
	ldd r25,Y+31
	std Y+27,r25
	std Y+26,r24
	ldd r24,Y+26
	ldd r25,Y+27
/* #APP */
 ;  105 "c:/program files/winavr-20100110/lib/gcc/../../avr/include/util/delay_basic.h" 1
	1: sbiw r24,1
	brne 1b
 ;  0 "" 2
/* #NOAPP */
	std Y+27,r25
	std Y+26,r24
	rjmp .L71
.L112:
	ldd r22,Y+41
	ldd r23,Y+42
	ldd r24,Y+43
	ldd r25,Y+44
	call __fixunssfsi
	movw r26,r24
	movw r24,r22
	std Y+40,r24
.L60:
	ldd r24,Y+40
	std Y+25,r24
	ldd r24,Y+25
/* #APP */
 ;  83 "c:/program files/winavr-20100110/lib/gcc/../../avr/include/util/delay_basic.h" 1
	1: dec r24
	brne 1b
 ;  0 "" 2
/* #NOAPP */
	std Y+25,r24
.L71:
	ldi r26,lo8(50)
	ldi r27,hi8(50)
	ldi r30,lo8(50)
	ldi r31,hi8(50)
	ld r24,Z
	ori r24,lo8(2)
	st X,r24
	ldi r26,lo8(50)
	ldi r27,hi8(50)
	ldi r30,lo8(50)
	ldi r31,hi8(50)
	ld r24,Z
	andi r24,lo8(-5)
	st X,r24
/* #APP */
 ;  112 "z:\PM\lab3\lab3.c" 1
	nop;nop;nop;
 ;  0 "" 2
/* #NOAPP */
	ldi r26,lo8(50)
	ldi r27,hi8(50)
	ldi r30,lo8(50)
	ldi r31,hi8(50)
	ld r24,Z
	andi r24,lo8(-3)
	st X,r24
	ldi r24,lo8(0x42200000)
	ldi r25,hi8(0x42200000)
	ldi r26,hlo8(0x42200000)
	ldi r27,hhi8(0x42200000)
	std Y+21,r24
	std Y+22,r25
	std Y+23,r26
	std Y+24,r27
	ldd r22,Y+21
	ldd r23,Y+22
	ldd r24,Y+23
	ldd r25,Y+24
	ldi r18,lo8(0x402aaaab)
	ldi r19,hi8(0x402aaaab)
	ldi r20,hlo8(0x402aaaab)
	ldi r21,hhi8(0x402aaaab)
	call __mulsf3
	movw r26,r24
	movw r24,r22
	std Y+17,r24
	std Y+18,r25
	std Y+19,r26
	std Y+20,r27
	ldd r22,Y+17
	ldd r23,Y+18
	ldd r24,Y+19
	ldd r25,Y+20
	ldi r18,lo8(0x3f800000)
	ldi r19,hi8(0x3f800000)
	ldi r20,hlo8(0x3f800000)
	ldi r21,hhi8(0x3f800000)
	call __ltsf2
	tst r24
	brge .L115
.L99:
	ldi r24,lo8(1)
	std Y+16,r24
	rjmp .L74
.L115:
	ldd r22,Y+17
	ldd r23,Y+18
	ldd r24,Y+19
	ldd r25,Y+20
	ldi r18,lo8(0x437f0000)
	ldi r19,hi8(0x437f0000)
	ldi r20,hlo8(0x437f0000)
	ldi r21,hhi8(0x437f0000)
	call __gtsf2
	cp __zero_reg__,r24
	brlt .+2
	rjmp .L116
.L100:
	ldd r22,Y+21
	ldd r23,Y+22
	ldd r24,Y+23
	ldd r25,Y+24
	ldi r18,lo8(0x447a0000)
	ldi r19,hi8(0x447a0000)
	ldi r20,hlo8(0x447a0000)
	ldi r21,hhi8(0x447a0000)
	call __divsf3
	movw r26,r24
	movw r24,r22
	std Y+12,r24
	std Y+13,r25
	std Y+14,r26
	std Y+15,r27
	ldd r22,Y+12
	ldd r23,Y+13
	ldd r24,Y+14
	ldd r25,Y+15
	ldi r18,lo8(0x44fa0000)
	ldi r19,hi8(0x44fa0000)
	ldi r20,hlo8(0x44fa0000)
	ldi r21,hhi8(0x44fa0000)
	call __mulsf3
	movw r26,r24
	movw r24,r22
	std Y+8,r24
	std Y+9,r25
	std Y+10,r26
	std Y+11,r27
	ldd r22,Y+8
	ldd r23,Y+9
	ldd r24,Y+10
	ldd r25,Y+11
	ldi r18,lo8(0x3f800000)
	ldi r19,hi8(0x3f800000)
	ldi r20,hlo8(0x3f800000)
	ldi r21,hhi8(0x3f800000)
	call __ltsf2
	tst r24
	brge .L117
.L101:
	ldi r24,lo8(1)
	ldi r25,hi8(1)
	std Y+7,r25
	std Y+6,r24
	rjmp .L79
.L117:
	ldd r22,Y+8
	ldd r23,Y+9
	ldd r24,Y+10
	ldd r25,Y+11
	ldi r18,lo8(0x477fff00)
	ldi r19,hi8(0x477fff00)
	ldi r20,hlo8(0x477fff00)
	ldi r21,hhi8(0x477fff00)
	call __gtsf2
	cp __zero_reg__,r24
	brge .L118
.L102:
	ldd r22,Y+12
	ldd r23,Y+13
	ldd r24,Y+14
	ldd r25,Y+15
	ldi r18,lo8(0x41200000)
	ldi r19,hi8(0x41200000)
	ldi r20,hlo8(0x41200000)
	ldi r21,hhi8(0x41200000)
	call __mulsf3
	movw r26,r24
	movw r24,r22
	movw r22,r24
	movw r24,r26
	call __fixunssfsi
	movw r26,r24
	movw r24,r22
	std Y+7,r25
	std Y+6,r24
	rjmp .L82
.L83:
	ldi r24,lo8(200)
	ldi r25,hi8(200)
	std Y+5,r25
	std Y+4,r24
	ldd r24,Y+4
	ldd r25,Y+5
/* #APP */
 ;  105 "c:/program files/winavr-20100110/lib/gcc/../../avr/include/util/delay_basic.h" 1
	1: sbiw r24,1
	brne 1b
 ;  0 "" 2
/* #NOAPP */
	std Y+5,r25
	std Y+4,r24
	ldd r24,Y+6
	ldd r25,Y+7
	sbiw r24,1
	std Y+7,r25
	std Y+6,r24
.L82:
	ldd r24,Y+6
	ldd r25,Y+7
	sbiw r24,0
	brne .L83
	rjmp .L86
.L118:
	ldd r22,Y+8
	ldd r23,Y+9
	ldd r24,Y+10
	ldd r25,Y+11
	call __fixunssfsi
	movw r26,r24
	movw r24,r22
	std Y+7,r25
	std Y+6,r24
.L79:
	ldd r24,Y+6
	ldd r25,Y+7
	std Y+3,r25
	std Y+2,r24
	ldd r24,Y+2
	ldd r25,Y+3
/* #APP */
 ;  105 "c:/program files/winavr-20100110/lib/gcc/../../avr/include/util/delay_basic.h" 1
	1: sbiw r24,1
	brne 1b
 ;  0 "" 2
/* #NOAPP */
	std Y+3,r25
	std Y+2,r24
	rjmp .L86
.L116:
	ldd r22,Y+17
	ldd r23,Y+18
	ldd r24,Y+19
	ldd r25,Y+20
	call __fixunssfsi
	movw r26,r24
	movw r24,r22
	std Y+16,r24
.L74:
	ldd r24,Y+16
	std Y+1,r24
	ldd r24,Y+1
/* #APP */
 ;  83 "c:/program files/winavr-20100110/lib/gcc/../../avr/include/util/delay_basic.h" 1
	1: dec r24
	brne 1b
 ;  0 "" 2
/* #NOAPP */
	std Y+1,r24
.L86:
/* epilogue start */
	subi r28,lo8(-(100))
	sbci r29,hi8(-(100))
	in __tmp_reg__,__SREG__
	cli
	out __SP_H__,r29
	out __SREG__,__tmp_reg__
	out __SP_L__,r28
	pop r28
	pop r29
	pop r17
	pop r16
	ret
	.size	lcd_init, .-lcd_init
.global	main
	.type	main, @function
main:
	push r29
	push r28
	in r28,__SP_L__
	in r29,__SP_H__
	sbiw r28,28
	in __tmp_reg__,__SREG__
	cli
	out __SP_H__,r29
	out __SREG__,__tmp_reg__
	out __SP_L__,r28
/* prologue: function */
/* frame size = 28 */
	call lcd_init
	ldi r24,lo8(0x40000000)
	ldi r25,hi8(0x40000000)
	ldi r26,hlo8(0x40000000)
	ldi r27,hhi8(0x40000000)
	std Y+25,r24
	std Y+26,r25
	std Y+27,r26
	std Y+28,r27
	ldd r22,Y+25
	ldd r23,Y+26
	ldd r24,Y+27
	ldd r25,Y+28
	ldi r18,lo8(0x44fa0000)
	ldi r19,hi8(0x44fa0000)
	ldi r20,hlo8(0x44fa0000)
	ldi r21,hhi8(0x44fa0000)
	call __mulsf3
	movw r26,r24
	movw r24,r22
	std Y+21,r24
	std Y+22,r25
	std Y+23,r26
	std Y+24,r27
	ldd r22,Y+21
	ldd r23,Y+22
	ldd r24,Y+23
	ldd r25,Y+24
	ldi r18,lo8(0x3f800000)
	ldi r19,hi8(0x3f800000)
	ldi r20,hlo8(0x3f800000)
	ldi r21,hhi8(0x3f800000)
	call __ltsf2
	tst r24
	brge .L142
.L138:
	ldi r24,lo8(1)
	ldi r25,hi8(1)
	std Y+20,r25
	std Y+19,r24
	rjmp .L122
.L142:
	ldd r22,Y+21
	ldd r23,Y+22
	ldd r24,Y+23
	ldd r25,Y+24
	ldi r18,lo8(0x477fff00)
	ldi r19,hi8(0x477fff00)
	ldi r20,hlo8(0x477fff00)
	ldi r21,hhi8(0x477fff00)
	call __gtsf2
	cp __zero_reg__,r24
	brge .L143
.L139:
	ldd r22,Y+25
	ldd r23,Y+26
	ldd r24,Y+27
	ldd r25,Y+28
	ldi r18,lo8(0x41200000)
	ldi r19,hi8(0x41200000)
	ldi r20,hlo8(0x41200000)
	ldi r21,hhi8(0x41200000)
	call __mulsf3
	movw r26,r24
	movw r24,r22
	movw r22,r24
	movw r24,r26
	call __fixunssfsi
	movw r26,r24
	movw r24,r22
	std Y+20,r25
	std Y+19,r24
	rjmp .L125
.L126:
	ldi r24,lo8(200)
	ldi r25,hi8(200)
	std Y+18,r25
	std Y+17,r24
	ldd r24,Y+17
	ldd r25,Y+18
/* #APP */
 ;  105 "c:/program files/winavr-20100110/lib/gcc/../../avr/include/util/delay_basic.h" 1
	1: sbiw r24,1
	brne 1b
 ;  0 "" 2
/* #NOAPP */
	std Y+18,r25
	std Y+17,r24
	ldd r24,Y+19
	ldd r25,Y+20
	sbiw r24,1
	std Y+20,r25
	std Y+19,r24
.L125:
	ldd r24,Y+19
	ldd r25,Y+20
	sbiw r24,0
	brne .L126
	rjmp .L127
.L143:
	ldd r22,Y+21
	ldd r23,Y+22
	ldd r24,Y+23
	ldd r25,Y+24
	call __fixunssfsi
	movw r26,r24
	movw r24,r22
	std Y+20,r25
	std Y+19,r24
.L122:
	ldd r24,Y+19
	ldd r25,Y+20
	std Y+16,r25
	std Y+15,r24
	ldd r24,Y+15
	ldd r25,Y+16
/* #APP */
 ;  105 "c:/program files/winavr-20100110/lib/gcc/../../avr/include/util/delay_basic.h" 1
	1: sbiw r24,1
	brne 1b
 ;  0 "" 2
/* #NOAPP */
	std Y+16,r25
	std Y+15,r24
.L127:
	ldi r24,lo8(40)
	call send_command
	ldi r24,lo8(0x40000000)
	ldi r25,hi8(0x40000000)
	ldi r26,hlo8(0x40000000)
	ldi r27,hhi8(0x40000000)
	std Y+11,r24
	std Y+12,r25
	std Y+13,r26
	std Y+14,r27
	ldd r22,Y+11
	ldd r23,Y+12
	ldd r24,Y+13
	ldd r25,Y+14
	ldi r18,lo8(0x44fa0000)
	ldi r19,hi8(0x44fa0000)
	ldi r20,hlo8(0x44fa0000)
	ldi r21,hhi8(0x44fa0000)
	call __mulsf3
	movw r26,r24
	movw r24,r22
	std Y+7,r24
	std Y+8,r25
	std Y+9,r26
	std Y+10,r27
	ldd r22,Y+7
	ldd r23,Y+8
	ldd r24,Y+9
	ldd r25,Y+10
	ldi r18,lo8(0x3f800000)
	ldi r19,hi8(0x3f800000)
	ldi r20,hlo8(0x3f800000)
	ldi r21,hhi8(0x3f800000)
	call __ltsf2
	tst r24
	brge .L144
.L140:
	ldi r24,lo8(1)
	ldi r25,hi8(1)
	std Y+6,r25
	std Y+5,r24
	rjmp .L130
.L144:
	ldd r22,Y+7
	ldd r23,Y+8
	ldd r24,Y+9
	ldd r25,Y+10
	ldi r18,lo8(0x477fff00)
	ldi r19,hi8(0x477fff00)
	ldi r20,hlo8(0x477fff00)
	ldi r21,hhi8(0x477fff00)
	call __gtsf2
	cp __zero_reg__,r24
	brge .L145
.L141:
	ldd r22,Y+11
	ldd r23,Y+12
	ldd r24,Y+13
	ldd r25,Y+14
	ldi r18,lo8(0x41200000)
	ldi r19,hi8(0x41200000)
	ldi r20,hlo8(0x41200000)
	ldi r21,hhi8(0x41200000)
	call __mulsf3
	movw r26,r24
	movw r24,r22
	movw r22,r24
	movw r24,r26
	call __fixunssfsi
	movw r26,r24
	movw r24,r22
	std Y+6,r25
	std Y+5,r24
	rjmp .L133
.L134:
	ldi r24,lo8(200)
	ldi r25,hi8(200)
	std Y+4,r25
	std Y+3,r24
	ldd r24,Y+3
	ldd r25,Y+4
/* #APP */
 ;  105 "c:/program files/winavr-20100110/lib/gcc/../../avr/include/util/delay_basic.h" 1
	1: sbiw r24,1
	brne 1b
 ;  0 "" 2
/* #NOAPP */
	std Y+4,r25
	std Y+3,r24
	ldd r24,Y+5
	ldd r25,Y+6
	sbiw r24,1
	std Y+6,r25
	std Y+5,r24
.L133:
	ldd r24,Y+5
	ldd r25,Y+6
	sbiw r24,0
	brne .L134
	rjmp .L135
.L145:
	ldd r22,Y+7
	ldd r23,Y+8
	ldd r24,Y+9
	ldd r25,Y+10
	call __fixunssfsi
	movw r26,r24
	movw r24,r22
	std Y+6,r25
	std Y+5,r24
.L130:
	ldd r24,Y+5
	ldd r25,Y+6
	std Y+2,r25
	std Y+1,r24
	ldd r24,Y+1
	ldd r25,Y+2
/* #APP */
 ;  105 "c:/program files/winavr-20100110/lib/gcc/../../avr/include/util/delay_basic.h" 1
	1: sbiw r24,1
	brne 1b
 ;  0 "" 2
/* #NOAPP */
	std Y+2,r25
	std Y+1,r24
.L135:
.L136:
	rjmp .L136
	.size	main, .-main
