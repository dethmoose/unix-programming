.data
    .equ Stdout, 1        @ Select stdout to write to
    .equ SWI_PrInt, 0x6b  @ Write an Integer
    .equ SWI_PrStr, 0x69  @ Write a String
    .equ SWI_Exit,  0x11  @ Stop execution

factorialEquals: .asciz "! = "
newLine:         .asciz "\n"



.text
.global _start

/* TODO: rewrite ARM code into AT&T syntax */

/*
factorial(n):
    if n == 1:
        return n
    return n * factorial(n-1)
*/



factorial:
    PUSH { r4, lr }
    CMP  r0, #1
    BEQ  baseCase
    MOV  r4, r0
    SUB  r0, r0, #1
    BL   factorial
    MUL  r4, r4, r0
    BAL  lEnd
baseCase:
    MOV  r4, #1
lEnd:
    MOV  r0, r4
    POP  { r4, pc }



_start:
    MOV  r3, #1                @ n parameter, initial value = 1
mainLoop:
    MOV  r0, r3
    BL   factorial
    MOV  r2, r0                @ save result from factorial

    MOV  r1, r3                @ print n
    MOV  r0, #Stdout
    SWI  SWI_PrInt

    LDR  r1, =factorialEquals  @ print string
    MOV  r0, #Stdout
    SWI  SWI_PrStr

    MOV  r1, r2                @ print result from factorial
    MOV  r0, #Stdout
    SWI  SWI_PrInt

    LDR  r1, =newLine          @ print newline  
    MOV  r0, #Stdout
    SWI  SWI_PrStr     

    ADD  r3, r3, #1            @ increment n
    CMP  r3, #11
    BNE  mainLoop              @ repeat while n <= 10
    SWI  SWI_Exit
