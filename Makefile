output: main.o lex.o grammar.o create.o compiler.o codegen.o opcode.o
	$(CC) -o bin/main main.o lex.o grammar.o create.o compiler.o codegen.o opcode.o
main.o : main/main.c
	$(CC) -c main/main.c
codegen.o : compiler/codegen.c
	$(CC) -c compiler/codegen.c -o codegen.o
opcode.o : compiler/opcode.c
	$(CC) -c compiler/opcode.c -o opcode.o
compiler.o : compiler/summoner.h
	$(CC) -c compiler/compiler.c -o compiler.o
create.o : compiler/summoner.h
	$(CC) -c compiler/create.c -o create.o
grammar.o : compiler/grammar.y compiler/summoner.h
	yacc -d compiler/grammar.y
	$(CC) -c y.tab.c -o grammar.o
lex.o : compiler/lex.l grammar.o
	lex compiler/lex.l
	$(CC) -c lex.yy.c -o lex.o

clean:
	rm y.tab.*
	rm lex.yy.c
	rm *.o
