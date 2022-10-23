    .data
resMsg: .asciz   "fak=%d\n"
buf:    .asciz   "xxxxxxxxx"
endMsg: .asciz   "slut\n"

    .text
    .global main

main:
    pushq   $0              # Stacken ska vara 16 bytes ”aligned”
    movq    $5,%rdi         # Beräkna 5!
    call    fac
    movq    %rax,%rsi       # Flytta returvärdet till argumentregistret
    movq    $resMsg,%rdi    # skriv ut fak=”resultat”
    call    printf
    movq    $endMsg,%rdi    # följd av slut
    call    printf
    popq    %rax
    ret



# Här finns funktionen fac = n! (rekursiv)
fac:
    cmpq    $1,%rdi         # if n>1
    jle     lBase
    pushq   %rdi            # lägg anropsvärde på stacken
    decq    %rdi            # räkna ned värdet med 1
    call    fac             # temp = fakultet av (n-1)
    popq    %rdi            # hämta från stack
    imul    %rdi,%rax       # return n*temp
    ret
lBase:
    movq    $1,%rax         # else return 1
    ret
