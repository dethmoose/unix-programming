#!/usr/bin/bash
in_filename=$1
filename="output.s"

usage() { echo -e "One argument expected (input calc file)\nUsage: $0 [filename]"; exit 1; }

if [ $# -ne 1 ]; then
    usage
fi

# Create prologue (.bss, .data and .text segments)
ALPHA="a b c d e f g h i j k l m n o p q r s t u v w x y z"
echo -e "\t.bss" > $filename
for a in $ALPHA
do
    echo -e "$a:\t.quad\t0" >> $filename
done

echo -e "\n\t.data"                >> $filename
echo -e "fmt:\t.asciz\t\"%d\\\n\"" >> $filename   # format str for printf

echo -e "\n\t.text"                >> $filename
echo -e "\t.global\tmain\n"        >> $filename

echo    "# FACT"              >> $filename
echo    "fact:"               >> $filename
echo -e "\tcmpq\t\$1, %rdi"   >> $filename
echo -e "\tjle\t\tbase"       >> $filename
echo -e "\tpushq\t%rdi"       >> $filename
echo -e "\tdecq\t%rdi"        >> $filename
echo -e "\tcall\tfact"        >> $filename
echo -e "\tpopq\t%rdi"        >> $filename
echo -e "\timulq\t%rdi, %rax" >> $filename
echo -e "\tret"               >> $filename
echo    "base:"               >> $filename
echo -e "\tmovq\t\$1, %rax"   >> $filename
echo -e "\tret\n"             >> $filename

echo    "# GCD"              >> $filename
echo    "gcd:"               >> $filename
echo -e "\tcmpq\t%rsi, %rdi" >> $filename
echo -e "\tje\t\tequal"      >> $filename
echo -e "\tjl\t\trdiLess"    >> $filename
echo -e "\tsubq\t%rsi, %rdi" >> $filename
echo -e "\tjmp\t\tagain"     >> $filename
echo    "rdiLess:"           >> $filename
echo -e "\tsubq\t%rdi, %rsi" >> $filename
echo    "again:"             >> $filename
echo -e "\tjmp\t\tgcd"       >> $filename
echo    "equal:"             >> $filename
echo -e "\tmovq\t%rdi, %rax" >> $filename
echo -e "\tret\n"            >> $filename

echo    "# MAIN"       >> $filename
echo    "main:"        >> $filename
echo -e "\tpushq\t\$0" >> $filename   # align stack 16 bytes

# Execute with calc file
make all
(./bin/calc3y.exe < $in_filename) >> $filename

# Create epilogue
echo    "lExit:"             >> $filename
echo -e "\tmovq\t\$60,%rax"  >> $filename   # sys_exit has code 60
echo -e "\txor\t\t%rdi,%rdi" >> $filename   # exit code 0
echo -e "\tsyscall"          >> $filename

# Assemble and produce an executable
# TODO: link lib
gcc -no-pie -fPIC output.s -o output
./output

# Expected outcome:
# For example, when I run your shell script as follows:
# ’x86-64-driver.sh bcd.calc’, I expect as output a file called
# ’bcd.s’, which contains the produced x86-64 assembler code for the
# file ’bcd.calc’, as well as a file called ’bcd’, which is an
# executable program, which does what was written in ’bcd.calc’.
# TODO: change $filename to $in_filename without the file ending
