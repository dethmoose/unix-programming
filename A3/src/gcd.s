# Greatest Common Divisor

    .text
    .global gcd

# Input values in %rdi & %rsi, return value in %rax
gcd:
    cmpq    $0, %rsi
    jge     rdiNeg
    negq    %rsi
rdiNeg:
    cmpq    $0, %rdi
    jge     start
    negq    %rdi
start:
    cmpq    %rsi, %rdi
    je      equal
    jl      rdiLess
    subq    %rsi, %rdi
    jmp     again
rdiLess:
    subq    %rdi, %rsi
again:
    jmp     start
equal:
    movq    %rdi, %rax
    ret
