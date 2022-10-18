# Assignment 3

Deadline 2022-10-30.

Change calc3b.c into calc3i.c that instead of emitting the pseudo assembly code emits actual x86-64 assembler instructions.

Write a shell script, called `x86-64-driver.sh`, which takes as input a file with the `.calc` ending and then writes the required prologue into a new file with the ending `.s`, appends this file with the output of your compiler (i.e., `calc3i.exe`) for the given `.calc` file, and appends an epilogue.

Testa output från calc3b med `./bin/calc3b.exe < test` 
Skapa bibliotek genom att kompilera med `ar`
Skapa en Makefile

## Todo list

### Grade D

-   Implement compiler for basic stack machine.

-   Implement one additional function

## Questions
