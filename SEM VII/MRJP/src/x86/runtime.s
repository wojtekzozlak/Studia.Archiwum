.global .emptyVal
.emptyVal:
	.byte	0


.global error
error:
	push	%ebp
	movl	%esp, %ebp

	subl	$8, %esp
	movl	$1, (%esp)
	call	exit

	leave
	ret

.global .stringarray_init
.stringarray_init:
	push	%ebp
	movl	%esp, %ebp
	movl	$0, -4(%ebp)

.stringarray_init_cond:
	movl	-4(%ebp), %edx
	movl	12(%ebp), %ecx
	cmp	%edx, %ecx
	jz	.stringarray_init_end

.stringarray_init_body:
	movl	-4(%ebp), %edx
	movl	8(%ebp), %ecx
	movl	$.emptyVal, (%ecx, %edx, 4)
	incl	-4(%ebp)
	jmp	.stringarray_init_cond

.stringarray_init_end:
	leave
	ret

.global .concat
.concat:
	push	%ebp
	movl	%esp, %ebp

	subl	$40, %esp

	movl	$1, -4(%ebp)
	movl	$0, -8(%ebp)

	movl	8(%ebp), %edx
	movl	%edx, (%esp)
	call	strlen
	addl	%eax, -4(%ebp)

	movl	12(%ebp), %edx
	movl	%edx, (%esp)
	call	strlen
	addl	%eax, -4(%ebp)


	movl	$1, 4(%esp)
	movl	-4(%ebp), %edx
	movl	%edx, (%esp)
	call	calloc
	movl	%eax, -8(%ebp)

	movl	-8(%ebp), %edx
	movl	%edx, (%esp)
	movl	8(%ebp), %edx
	movl	%edx, 4(%esp)
	call	strcpy 

	movl	-8(%ebp), %edx
	movl	%edx, (%esp)
	movl	12(%ebp), %edx
	movl	%edx, 4(%esp)
	call	strcat

	movl	-8(%ebp), %eax

	leave
	ret


.global readString
readString:

	push	%ebp
	movl	%esp, %ebp

	movl	$0, -4(%ebp)
	movl	$0, -8(%ebp)

	subl	$24, %esp
	leal	-4(%ebp), %edi
	movl	%edi, (%esp)
	leal	-8(%ebp), %edi
	movl	%edi, 4(%esp)
	movl	stdin, %edi
	movl	%edi, 8(%esp)
	call	getline

	subl	$1, %eax
	movl	-4(%ebp), %edi
	movl	$0, (%edi, %eax)

	movl	-4(%ebp), %eax

	leave
	ret


.global printString
printString:
	push	%ebp
	movl	%esp, %ebp

	subl	$8, %esp
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	puts

	leave
	ret


.global printInt
printInt:
	push	%ebp
	movl	%esp, %ebp

	addl	$-16, %esp
	subl	$32, %esp
	movl	$.printIntPattern, %edi
	movl	%edi, (%esp)
	movl	8(%ebp), %edi
	movl	%edi, 4(%esp)
	call	printf

	leave
	ret

.global readInt
readInt:
	push	%ebp
	movl	%esp, %ebp

	subl	$24, %esp
	call	readString
	movl	%eax, -4(%ebp)

	movl	-4(%ebp), %edi
	movl	%edi, (%esp)
	movl	$.readIntPattern, %edi
	movl	%edi, 4(%esp)
	leal	-8(%ebp), %eax
	movl	%eax, 8(%esp)
	call	sscanf

	movl	-8(%ebp), %eax

	leave
	ret

.printIntPattern:
	.string "%d\n"
.readIntPattern:
	.string "%d"
