%{
#include <stdio.h>
#include "y.tab.h"
%}

%%
"+"  return ADD;
"-"  return SUB;
"*"  return MUL;
"/"  return DIV;
"("  return LP;
")"  return RP;
"\n" return CR;

([1-9][0-9]*)|0|([0-9]+\.[0-9]+) {
    double val;
    sscanf(yytext, "%lf", &val);
    yylval.double_value = val;
    return DOUBLE_LITERAL;
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



