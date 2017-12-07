CC=clang++
CFLAGS=-I.

rxxr2c: nfa.o parsingdata.o baselib.o
	$(CC) -o rxxr2c nfa.cpp parsingdata.cpp baselib.cpp -I.
