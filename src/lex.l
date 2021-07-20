%{
#include <stdio.h>
#include <string.h>
#include "y.tab.h"
%}

%%

[+\-*/\(\)<>!{}\n] {
    return *yytext;
}

">="       return GE;
"<="       return LE;
"=="       return EQ;
"!="       return NE;
"&&"       return AND;
"||"       return OR;
"if"       return IF;
"else"     return ELSE;
"for"      return FOR;
"nil"      return NIL;
"func"     return FUNCTION;
"return"   return RETURN;
"break"    return BREAK;
"continue" return CONTINUE;

"true"|"false" {
    yylval.int_value = strcmp(yytext, "false");
    return BOOL_LITERAL;
}

[0-9]+\.[0-9]+ {
    double val;
    sscanf(yytext, "%lf", &val);
    yylval.double_value = val;
    return DOUBLE_LITERAL;
}

([1-9][0-9]*)|0 {
    int val;
    sscanf(yytext, "%d", &val);
    yylval.int_value = val;
    return INT_LITERAL;
}

[A-Za-z_][A-Za-z_0-9]* {
  char *new_str = malloc(strlen(yytext) + 1);
  strcpy(new_str, yytext);
  yylval.identifier = new_str;
  return IDENTIFIER;
}

[ \t] ;

. {
    fprintf(stderr, "lexical error\n");
    exit(1);
}
%%

int yywrap(void) {
    return 1;
}
