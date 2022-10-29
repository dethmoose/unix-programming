# Binary Logarithm

    .text
    .global lntwo

# Input value in %rdi, return value in %rax
lntwo:
    movq    %rdi, %rax
    ret
