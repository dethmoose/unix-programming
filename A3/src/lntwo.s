# Binary Logarithm
# Take one argument and return the binary log

    .text
    .global lntwo

# Input value in %rdi, return value in %rax
lntwo:
    movq    %rdi, %rax
    ret
