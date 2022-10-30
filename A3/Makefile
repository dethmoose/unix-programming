all:
	gcc ./lexyacc-code/y.tab.o ./lexyacc-code/lex.yy.o ./lexyacc-code/calc3a.c -o ./bin/calc3a.exe
	gcc ./lexyacc-code/y.tab.o ./lexyacc-code/lex.yy.o ./lexyacc-code/calc3b.c -o ./bin/calc3b.exe
	gcc ./lexyacc-code/y.tab.o ./lexyacc-code/lex.yy.o ./lexyacc-code/calc3g.c -o ./bin/calc3g.exe
	gcc ./lexyacc-code/y.tab.o ./lexyacc-code/lex.yy.o ./lexyacc-code/calc3i.c -o ./bin/calc3i.exe
	gcc -c  ./src/gcd.s  -o ./build/gcd.o
	gcc -c  ./src/fact.s -o ./build/fact.o
	ar -crs ./lib/libutil.a ./build/fact.o ./build/gcd.o
