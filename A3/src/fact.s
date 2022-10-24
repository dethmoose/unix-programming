# Replace .s file in library with `ar` program from A3/:
# `ar -rsv ./lib/lib.a ./src/fact.s`

# List all modules within library:
# `ar -t ./lib/lib.a`

# Factorial
    .data
    .text
    .global fact

# Input value in %rdi, return value in %rax
fact:
    cmpq    $1,%rdi         # if n>1
    jle     base
    pushq   %rdi            # push arg to stack
    decq    %rdi
    call    fac             # temp = fact of (n-1)
    popq    %rdi            # pop from stack
    imulq   %rdi,%rax       # return n*temp
    ret
base:
    movq    $1,%rax         # else return 1
    ret
