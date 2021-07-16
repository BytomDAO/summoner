%{
#include <stdio.h>
#include "y.tab.h"
%}

%%

[+\-*/\(\)\n] {
    return *yytext;
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

[ \t] ;

. {
    fprintf(stderr, "lexical error\n");
    exit(1);
}
%%

int yywrap(void) {
    return 1;
}
