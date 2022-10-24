# Replace .s file in library with `ar` program from A3/:
# `ar -rsv ./lib/lib.a ./src/gcd.s`

# List all modules within library:
# `ar -t ./lib/lib.a`

# Greatest Common Divisor
    .data
    .text
    .global gcd

# Input values in %rdi & %rsi, return value in %rax
gcd:
    # Take two arguments
    # Return the gcd
    # For example: 36 gcd 24 = 12
    movq    $0,%rax
    ret
