comp:
	lex compiler/lex.l
	yacc -d compiler/grammar.y
	gcc -o bin/main y.tab.c lex.yy.c compiler/create.c compiler/compiler.c compiler/codegen.c main/main.c
	