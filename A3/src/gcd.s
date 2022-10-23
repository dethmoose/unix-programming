# Create library with `ar` program:
# `ar -crs lib.a gcd.s`

# List all modules within library:
# `ar -t lib.a`

# Greatest Common Divisor
    .data
    .text
    .global gcd

# Takes input value n in %rdi, return value in %rax
gcd:
    # Take two arguments
    # Return the gcd
    # For example: 36 gcd 24 = 12
    movq    $0,%rax
    ret
