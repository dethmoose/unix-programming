# Factorial

    .text
    .global fact

# Input value in %rdi, return value in %rax
fact:
    cmpq    $1, %rdi        # if n>1
    jle     base
    pushq   %rdi            # push arg to stack
    decq    %rdi
    call    fact            # temp = fact of (n-1)
    popq    %rdi            # pop from stack
    imulq   %rdi, %rax      # return n*temp
    ret

base:
    movq    $1, %rax        # else return 1
    ret
