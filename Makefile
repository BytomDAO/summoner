comp:
	lex src/lex.l
	yacc -d src/grammar.y
	gcc -o bin/main y.tab.c lex.yy.c src/create.c src/interpreter.c src/compiler.c src/codegen.c main/main.c
