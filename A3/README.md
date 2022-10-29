# Assignment 3

Deadline 2022-10-30.

Change calc3b.c into calc3i.c that instead of emitting the pseudo assembly code emits actual x86-64 assembler instructions.

Write a shell script, called `x86-64-driver.sh`, which takes as input a file with the `.calc` ending and then writes the required prologue into a new file with the ending `.s`, appends this file with the output of your compiler (i.e., `calc3i.exe`) for the given `.calc` file, and appends an epilogue.

## How to

### Run 
- `./x86-64-driver.sh [calc file]`, e.g. `./x86-64-driver.sh ./testprogs/gcd.calc`

### Test output from .exe file
- `make`
- `./bin/calc3i.exe < ./testprogs/test`

### Run output.s
- `gcc -no-pie -fPIC output.s -o output`
- `./output`

### Debug output.s with gdb
- `gcc -no-pie -fPIC output.s -o output -g`
- `gdb output`

### Create library `util`, or replace .s file in library
- `ar -crsv ./lib/libutil.a ./src/filename.s`

### List all modules within library
- `ar -t ./lib/libutil.a`

## Todo list

### Grade D

-   Implement compiler for basic stack machine

-   Implement one additional function

### Grade C

-   Implement the remaining two additional functions

-   Implement all three additional functions as proper external library (lib.a linked with gcc/ld)

### Grade B

-   Use system calls instead of library functions (`printf`) to handle output

### Grade A

-   Implement the compiler for the C stack machine

## Questions
