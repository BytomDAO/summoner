comp:
	lex src/lex.l
	yacc -d src/grammar.y
	gcc -o bin/main y.tab.c lex.yy.c src/summoner.c src/interpreter.c
