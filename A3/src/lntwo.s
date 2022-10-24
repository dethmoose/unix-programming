# Replace .s file in library with `ar` program from A3/:
# `ar -rsv ./lib/lib.a ./src/lntwo.s`

# List all modules within library:
# `ar -t ./lib/lib.a`

# Binary Logarithm
    .data
    .text
    .global lntwo

# Input value in %rdi, return value in %rax
lntwo:
    # Take one argument
    # Return the binary log 
    # For example: lntwo 32 = 5
    movq    $0,%rax
    ret
