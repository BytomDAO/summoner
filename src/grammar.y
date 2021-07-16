%{
#include <stdio.h>
#include <stdlib.h>
#include "src/summoner.h"
#include "src/interpreter.h"

int yylex();
int yyerror(const char *s);
%}

%union {
    double      double_value;
    int         int_value;
    struct Expression* expression;
}

%token <double_value> DOUBLE_LITERAL
%token <int_value> INT_LITERAL
%token '\n' '(' ')'
%left '+' '-'
%left '*' '/'
%nonassoc MINUS

%type <expression> expr

%%
stmt_list:
          stmt
        | stmt_list stmt
        ;

stmt:
     expr '\n' { printExprValue(evalExpression($1)); }

expr:
           INT_LITERAL           { $$ = allocIntExpression($1); }
         | DOUBLE_LITERAL        { $$ = allocDoubleExpression($1); }
         | expr '+' expr         { $$ = allocBinaryExpression(ADD_EXPRESSION, $1, $3); }
         | expr '-' expr         { $$ = allocBinaryExpression(SUB_EXPRESSION, $1, $3); }
         | expr '*' expr         { $$ = allocBinaryExpression(MUL_EXPRESSION, $1, $3); }
         | expr '/' expr         { $$ = allocBinaryExpression(DIV_EXPRESSION, $1, $3); }
         | '(' expr ')'          { $$ = $2; }
         | '-' expr %prec MINUS { $$ = allocUnaryExpression(MINUS_EXPRESSION, $2); }
         ;
%%

int yyerror(char const *str) {
    extern char *yytext;
    fprintf(stderr, "parse error near %s\n", yytext);
    return 0;
}

int main(void) {
    if (yyparse()) {
        exit(1);
    }
    return 0;
}
