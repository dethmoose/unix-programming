# ar - program
    .data
    .text
    .global fact

fact:
    cmpq    $1,%rdi         # if n>1
    jle     base
    pushq   %rdi            # lägg anropsvärde på stacken
    decq    %rdi            # räkna ned värdet med 1
    call    fac             # temp = fakultet av (n-1)
    popq    %rdi            # hämta från stack
    imulq   %rdi,%rax       # return n*temp
    ret
base:
    movq    $1,%rax         # else return 1
    ret
