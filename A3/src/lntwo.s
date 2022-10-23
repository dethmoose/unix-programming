# Create library with `ar` program:
# `ar -crs lib.a lntwo.s`

# List all modules within library:
# `ar -t lib.a`

# Binary Logarithm
    .data
    .text
    .global lntwo

# Takes input value n in %rdi, return value in %rax
lntwo:
    # Take one argument
    # Return the binary log 
    # For example: lntwo 32 = 5
    movq    $0,%rax
    ret
