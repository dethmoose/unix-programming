in_filename=$1  
filename="output.s"

ALPHA="a b c d e f g h i j k l m n o p q r s t u v w x y z"
echo ".bss" > $filename
for a in $ALPHA
do
    echo "$a:   .quad   0" >> $filename
done

# Command to parse calc file, and compile...

echo "  exit" >> $filename
