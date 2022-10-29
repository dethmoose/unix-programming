# Greatest Common Divisor
# Take two arguments and return the gcd

    .text
    .global gcd

# Input values in %rdi & %rsi, return value in %rax
gcd:
    cmpq    %rsi, %rdi
    je      equal
    jl      rdiLess
    subq    %rsi, %rdi
    jmp     again
rdiLess:
    subq    %rdi, %rsi
again:
    jmp     gcd
equal:
    movq    %rdi, %rax
    ret
