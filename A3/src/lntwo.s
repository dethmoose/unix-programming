# Binary Logarithm
# Takes a positive integer as input

    .text
    .global lntwo

# Input value in %rdi, return value in %rax
lntwo:
    movq    $0, %rax    # count how many shifts
    cmpq    $1, %rdi
    jle     done
shift:
    sar     $1, %rdi    # shift right = divide by 2
    incq    %rax
    cmpq    $1, %rdi
    jg      shift
done:
    ret
