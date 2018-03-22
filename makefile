CC=clang++
CFLAGS=-I.

rxxr2c: zanalyser.o y2analyser.o y1anlyser.o xanalyser.o product.o triple.o phi.o beta.o word.o flags.o nfa.o parsingdata.o baselib.o
	$(CC) -o rxxr2c zanalyser.cpp y2analyser.cpp y1analyser.cpp xanalyser.cpp product.cpp triple.cpp phi.cpp beta.cpp word.cpp flags.cpp nfa.cpp parsingdata.cpp baselib.cpp -I.
