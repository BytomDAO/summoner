output: main.o lex.o grammar.o create.o string.o compiler.o codegen.o opcode.o
	$(CC) -o bin/main $^
main.o : main/main.c
	$(CC) -c main/main.c
codegen.o : compiler/codegen.c
	$(CC) -c $< -o codegen.o
opcode.o : compiler/opcode.c
	$(CC) -c $< -o opcode.o
compiler.o : compiler/compiler.c
	$(CC) -c $< -o compiler.o
create.o : compiler/create.c
	$(CC) -c $< -o create.o
grammar.o : compiler/grammar.y
	yacc -d $<
	$(CC) -c y.tab.c -o grammar.o
lex.o : compiler/lex.l grammar.o
	lex $<
	$(CC) -c lex.yy.c -o lex.o
string.o : compiler/string.c
	$(CC) -c $< -o string.o

clean:
	rm -f y.tab.*
	rm -f lex.yy.c
	rm -f *.o
	rm -f bin/main
