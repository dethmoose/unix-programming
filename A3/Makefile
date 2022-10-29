all:
	gcc ./lexyacc-code/y.tab.o ./lexyacc-code/lex.yy.o ./lexyacc-code/calc3a.c -o ./bin/calc3a.exe
	gcc ./lexyacc-code/y.tab.o ./lexyacc-code/lex.yy.o ./lexyacc-code/calc3b.c -o ./bin/calc3b.exe
	gcc ./lexyacc-code/y.tab.o ./lexyacc-code/lex.yy.o ./lexyacc-code/calc3g.c -o ./bin/calc3g.exe
	gcc ./lexyacc-code/y.tab.o ./lexyacc-code/lex.yy.o ./lexyacc-code/calc3i.c -o ./bin/calc3i.exe
	ar -crs ./lib/libutil.a ./src/fact.s ./src/gcd.s
