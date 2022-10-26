# Assignment 3

Deadline 2022-10-30.

Change calc3b.c into calc3i.c that instead of emitting the pseudo assembly code emits actual x86-64 assembler instructions.

Write a shell script, called `x86-64-driver.sh`, which takes as input a file with the `.calc` ending and then writes the required prologue into a new file with the ending `.s`, appends this file with the output of your compiler (i.e., `calc3i.exe`) for the given `.calc` file, and appends an epilogue.

Testa output från calc3b med `./bin/calc3b.exe < test` 
Skapa bibliotek genom att kompilera med `ar`
Skapa en Makefile

## How to

- Call ./x86-64-driver.sh
- `gcc -no-pie -fPIC assemblytest.s -o assemblytest`

# To debug with gdb:
- `gcc -g -no-pie -fPIC assemblytest.s -o assemblytest`
- `gdb assemblytest`


## Todo list

### Grade D

-   Implement compiler for basic stack machine.

-   Implement one additional function

-   In calc3i.c, check union value of e.g. `p->opr.op[0]` (constant, identifier) to know whether to use `con->value (%d) id->i + 'a' (%c)`

## Questions
