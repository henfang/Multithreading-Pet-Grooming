# Henry Fang, Aasrija Puchakatla
# hef052, aap435
# CMPT332
# A3Q2

all: test.o petgroomsynch.o testmake

test.o: test.c petgroomsynch.h
	gcc -Wall -c test.c
petgroomsynch.o: petgroomsynch.c petgroomsynch.h
	gcc -Wall -c petgroomsynch.c
testmake:
	gcc -Wall -o test test.o petgroomsynch.o -pthread

clean:
	rm -f *.o
	rm -f test