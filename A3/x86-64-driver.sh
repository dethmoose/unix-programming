#!/usr/bin/bash

# Expected outcome:
# For example, when I run your shell script as follows:
# ’x86-64-driver.sh bcd.calc’, I expect as output a file called
# ’bcd.s’, which contains the produced x86-64 assembler code for the
# file ’bcd.calc’, as well as a file called ’bcd’, which is an
# executable program, which does what was written in ’bcd.calc’.

in_filepath=$1
# in_filename=$(basename $1 ".calc")
out_filename="./build/$(basename $1 ".calc")"
out_filepath="$out_filename.s"

usage() { echo -e "One argument expected (input calc file)\nUsage: $0 [filename]"; exit 1; }

if [ $# -ne 1 ]; then
    usage
fi

# Create prologue (.bss, .data and .text segments)
ALPHA="a b c d e f g h i j k l m n o p q r s t u v w x y z"
echo -e "\t.bss" > $out_filepath
for a in $ALPHA
do
    echo -e "$a:\t.quad\t0" >> $out_filepath
done

echo -e "\n\t.data"                >> $out_filepath
echo -e "fmt:\t.asciz\t\"%d\\\n\"" >> $out_filepath   # format str for printf

echo -e "\n\t.text"                >> $out_filepath
echo -e "\t.global\tmain\n"        >> $out_filepath

# Define util functions if not linking external lib
# define_fact
# define_gcd

echo    "main:"        >> $out_filepath
echo -e "\tpushq\t\$0" >> $out_filepath   # align stack 16 bytes

# Execute with calc file
make all && echo
(./bin/calc3i.exe < $in_filepath) >> $out_filepath

# Create epilogue
echo    "lExit:"             >> $out_filepath
echo -e "\tpopq\t%rax"       >> $out_filepath   # pop stack align
echo -e "\tmovq\t\$60,%rax"  >> $out_filepath   # sys_exit has code 60
echo -e "\txor\t\t%rdi,%rdi" >> $out_filepath   # exit code 0
echo -e "\tsyscall"          >> $out_filepath

# Assemble and produce an executable, then execute
gcc -no-pie -fPIC $out_filepath -o $out_filename -L ./lib -l util
$out_filename



# Helper functions for prologue
define_fact() {
    echo    "fact:"               >> $out_filepath
    echo -e "\tcmpq\t\$1, %rdi"   >> $out_filepath
    echo -e "\tjle\t\tbase"       >> $out_filepath
    echo -e "\tpushq\t%rdi"       >> $out_filepath
    echo -e "\tdecq\t%rdi"        >> $out_filepath
    echo -e "\tcall\tfact"        >> $out_filepath
    echo -e "\tpopq\t%rdi"        >> $out_filepath
    echo -e "\timulq\t%rdi, %rax" >> $out_filepath
    echo -e "\tret"               >> $out_filepath
    echo    "base:"               >> $out_filepath
    echo -e "\tmovq\t\$1, %rax"   >> $out_filepath
    echo -e "\tret\n"             >> $out_filepath
}

define_gcd() {
    echo    "gcd:"               >> $out_filepath
    echo -e "\tcmpq\t%rsi, %rdi" >> $out_filepath
    echo -e "\tje\t\tequal"      >> $out_filepath
    echo -e "\tjl\t\trdiLess"    >> $out_filepath
    echo -e "\tsubq\t%rsi, %rdi" >> $out_filepath
    echo -e "\tjmp\t\tagain"     >> $out_filepath
    echo    "rdiLess:"           >> $out_filepath
    echo -e "\tsubq\t%rdi, %rsi" >> $out_filepath
    echo    "again:"             >> $out_filepath
    echo -e "\tjmp\t\tgcd"       >> $out_filepath
    echo    "equal:"             >> $out_filepath
    echo -e "\tmovq\t%rdi, %rax" >> $out_filepath
    echo -e "\tret\n"            >> $out_filepath
}
