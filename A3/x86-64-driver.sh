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
(./bin/calc3b.exe < ./testprogs/gcd.calc) >> $filename

# Create epilogue
echo    "lExit:"             >> $filename
echo -e "\tmovq\t\$60,%rax"  >> $filename	# sys_exit has code 60
echo -e "\txor\t\t%rdi,%rdi" >> $filename	# exit code 0
echo -e "\tsyscall"          >> $filename
