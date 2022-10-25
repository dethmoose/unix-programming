# `gcc -no-pie -fPIC assemblytest.s -o assemblytest`

# To debug with gdb:
# `gcc -g -no-pie -fPIC assemblytest.s -o assemblytest`
# `gdb assemblytest`

# Set breakpoint for function:                        `b functionname`
# View registers:                                     `info registers` 
# Specific register (integer): `print $rax` , (hexa): `print/x $rax`
# Get update for register every time program pauses:  `display $rax` 

# Testing `A3/bin/calc3b.exe < A3/testprogs/gcd.calc`

    .data
a:		.quad		0
b:		.quad		0
aMsg: 	.asciz   	"a=%d\n"
bMsg: 	.asciz   	"b=%d\n"
endMsg: .asciz   	"end\n"
numMsg: .asciz      "%d\n"

    .text
    .global main

main:                   # 'a=732; b=2684;'
    pushq   $0       	# Stack 16 bytes ”aligned”
    pushq	$732
	popq	a
	pushq	$2684
	popq	b

L000:                   # 'while(a != b) {'
    pushq	a
    pushq	b
# 	TODO: compNE
    popq    %r10        # b
    popq    %r11        # a
    cmpq    %r10,%r11   # a-b
	jz		L001
	pushq	a
	pushq	b
# 	TODO: compGT          'if(a > b) {'
    popq    %r10        # b
    popq    %r11        # a
    cmpq    %r10,%r11   # a-b
#	jz		L002
#	pushq	a             '} else { b=b-a;'
#	pushq	b
# 	subq				# TODO subq S,D (D-S)
#	popq	a
#	jmp		L003

L002:                   # 'a=a-b;'
# 	pushq	b
# 	pushq	a
# 	subq				# TODO subq S,D (D-S)
# 	popq	b

L003:
#	jmp		L000          '}' (end of while and if-else)

L001:                    # 'print a; print a gcd b;'
# 	pushq	a
    movq    a,%rsi       # temp so that printf won't crash
    movq    $numMsg,%rdi # temp so that printf won't crash
	call	printf
# 	pushq	a
# 	pushq	b
	call	gcd
	call	printf
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
	movq	$244,%rsi	# 732 gcd 2684 = 244
	movq	$numMsg,%rdi
    ret
