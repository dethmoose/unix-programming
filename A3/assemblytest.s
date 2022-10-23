# To debug with gdb:
# `gcc -g -no-pie -fPIC assemblytest.s -o assemblytest`
# `gdb test`

# Set breakpoint for function:                        `b functionname`
# View registers:                                     `info registers` 
# Specific register (integer): `print $rax` , (hexa): `print/x $rax`
# Get update for register every time program pauses:  `display $rax` 

# Testing `A3/bin/calc3b.exe < A3/test` (gcd)
    .data
aMsg: 	.asciz   	"a=%d\n"
bMsg: 	.asciz   	"b=%d\n"
endMsg: .asciz   	"end\n"
a:		.quad		0
b:		.quad		0

    .text
    .global main

main:
    pushq   $0       	# Stack 16 bytes ”aligned”
    pushq	$732
	popq	a
	pushq	$2684
	popq	b

# L000:
# 	pushq	a
# 	pushq	b
# 	# TODO compNE
# 	jz		L001
# 	pushq	a
# 	pushq	b
# 	# TODO compGT
# 	jz		L002
# 	pushq	a
# 	pushq	b
# 	# subq				# TODO subq S,D (D-S)
# 	popq	a
# 	jmp		L003
# L002:
# 	pushq	b
# 	pushq	a
# 	# subq				# TODO subq S,D (D-S)
# 	popq	b
# L003:
# 	jmp		L000
# L001:
# 	pushq	a
# 	call	printf
# 	pushq	a
# 	pushq	b
# 	call	gcd
# 	call	printf
# 	popq	b

# debug print a & b
lPrint:
	movq	a,%rsi
	movq	$aMsg,%rdi
	call	printf
	movq	b,%rsi
	movq	$bMsg,%rdi
	call	printf
    movq    $endMsg,%rdi
    call    printf
lEnd:
    popq    %rax		# Stack align
    ret

# temp gcd
gcd:
	movq	$244,%rdi	# 732 gcd 2684 = 244
    ret
