%{
#include <stdio.h>
#include <string.h>
#include "y.tab.h"
%}

%x COMMENT
%%

"//"         BEGIN(COMMENT);
<COMMENT>\n  { BEGIN(0); return '\n'; }
<COMMENT>.   ; // eat anything in comment

[+\-*/\(\)<>!{}\n=,] {
    return *yytext;
}

":="       return DECL_ASSIGN;
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
"var"      return VAR;
"const"    return CONST;
"func"     return FUNCTION;
"return"   return RETURN;
"break"    return BREAK;
"continue" return CONTINUE;
"bool"     return BOOL_T;
"int"      return INT_T;
"double"   return DOUBLE_T;
"string"   return STRING_T;

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
  char *new_str = (char *)malloc(strlen(yytext) + 1);
  strcpy(new_str, yytext);
  yylval.identifier = new_str;
  return IDENTIFIER;
}

[ \t] ;

. {
    fprintf(stderr, "lexical error:%s\n", yytext);
    exit(1);
}
%%

int yywrap(void) {
    return 1;
}
