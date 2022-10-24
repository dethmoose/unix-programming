in_filename=$1  
filename="output.s"

# Create prologue (.bss and .text segments)
ALPHA="a b c d e f g h i j k l m n o p q r s t u v w x y z"
echo -e "\t.bss" > $filename
for a in $ALPHA
do
    echo -e "$a:\t.quad\t0" >> $filename
done
echo -e "\n\t.text"         >> $filename
echo -e "\t.global\tmain\n" >> $filename
echo    "main:"             >> $filename

# Command to parse calc file, and compile...

# Executing with test file
(./bin/calc3y.exe < $in_filename) >> $filename

# Create epilogue
echo    "lExit:"             >> $filename
echo -e "\tmovq\t\$60,%rax"  >> $filename	# sys_exit has code 60
echo -e "\txor\t\t%rdi,%rdi" >> $filename	# exit code 0
echo -e "\tsyscall"          >> $filename

# TODO
# Call ’gcc’ (or ’as’ and ’ld’ separately) to assemble
# and link the assembly file to produce an executable

# Expected outcome:
# For example, when I run your shell script as follows:
# ’x86-64-driver.sh bcd.calc’, I expect as output a file called
# ’bcd.s’, which contains the produced x86-64 assembler code for the
# file ’bcd.calc’, as well as a file called ’bcd’, which is an
# executable program, which does what was written in ’bcd.calc’.
