all:
	gcc ./lexyacc-code/y.tab.o ./lexyacc-code/lex.yy.o ./lexyacc-code/calc3i.c -o ./bin/calc3i.exe
	gcc -c ./src/gcd.s -o ./build/gcd.o
	gcc -c ./src/fact.s -o ./build/fact.o
	gcc -c ./src/lntwo.s -o ./build/lntwo.o
	ar -crs ./lib/libutil.a ./build/fact.o ./build/gcd.o ./build/lntwo.o

clean:
	rm -f ./build/* ./bin/*
