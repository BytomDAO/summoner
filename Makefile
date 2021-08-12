output: main.o lex.o grammar.o create.o string.o compiler.o builtin.o fix_tree.o codegen.o opcode.o error.o debug.o wchar.o
	$(CC) -o bin/main $^
main.o : main/main.c
	$(CC) -c main/main.c
codegen.o : compiler/codegen.c
	$(CC) -c $< -o codegen.o
opcode.o : compiler/opcode.c
	$(CC) -c $< -o opcode.o
compiler.o : compiler/compiler.c
	$(CC) -c $< -o compiler.o
fix_tree.o : compiler/fix_tree.c
	$(CC) -c $< -o fix_tree.o
builtin.o : builtin/builtin.c
	$(CC) -c $< -o builtin.o
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
wchar.o: share/wchar.c
	$(CC) -c $< -o wchar.o
error.o: compiler/error.c
	$(CC) -c $< -o error.o
debug.o: debug/debug.c
	$(CC) -c $< -o debug.o

clean:
	rm -f y.tab.*
	rm -f lex.yy.c
	rm -f *.o
	rm -f bin/main
