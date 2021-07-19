%{
#include <stdio.h>
#include <stdlib.h>
#include "src/summoner.h"
#include "src/interpreter.h"

int yylex();
int yyerror(const char *s);
%}

%union {
    char        *identifier;
    double      double_value;
    int         int_value;
    struct Expression* expression;
    struct Statement *statement;
}

%token <int_value> BOOL_LITERAL
%token <double_value> DOUBLE_LITERAL
%token <int_value> INT_LITERAL
%token <identifier> IDENTIFIER;

%nonassoc '\n'
%nonassoc '='
%left AND OR
%nonassoc EQ NE
%nonassoc '>' '<' LE GE
%left '+' '-'
%left '*' '/'
%nonassoc MINUS
%nonassoc NOT

%type <expression> expr
%type <statement> stmt

%%
stmt_list:
          stmt
        | stmt_list stmt
        ;

stmt:
       expr '\n'           { printExprValue(evalExpression($1)); }
     | IDENTIFIER '=' expr { $$ = allocAssignStatement($1, $3); }
     ;

expr:
           INT_LITERAL           { $$ = allocIntExpression($1); }
         | DOUBLE_LITERAL        { $$ = allocDoubleExpression($1); }
         | BOOL_LITERAL          { $$ = allocBoolExpression($1); }
         | IDENTIFIER            { $$ = allocIdentifierExpression($1); }
         | expr '+' expr         { $$ = allocBinaryExpression(ADD_EXPRESSION, $1, $3); }
         | expr '-' expr         { $$ = allocBinaryExpression(SUB_EXPRESSION, $1, $3); }
         | expr '*' expr         { $$ = allocBinaryExpression(MUL_EXPRESSION, $1, $3); }
         | expr '/' expr         { $$ = allocBinaryExpression(DIV_EXPRESSION, $1, $3); }
         | expr '>' expr         { $$ = allocBinaryExpression(GT_EXPRESSION, $1, $3); }
         | expr GE expr          { $$ = allocBinaryExpression(GE_EXPRESSION, $1, $3); }
         | expr '<' expr         { $$ = allocBinaryExpression(LT_EXPRESSION, $1, $3); }
         | expr LE expr          { $$ = allocBinaryExpression(LE_EXPRESSION, $1, $3); }
         | expr EQ expr          { $$ = allocBinaryExpression(EQ_EXPRESSION, $1, $3); }
         | expr NE expr          { $$ = allocBinaryExpression(NE_EXPRESSION, $1, $3); }
         | expr AND expr         { $$ = allocBinaryExpression(AND_EXPRESSION, $1, $3); }
         | expr OR expr          { $$ = allocBinaryExpression(OR_EXPRESSION, $1, $3); }
         | '!' expr %prec NOT    { $$ = allocUnaryExpression(NOT_EXPRESSION, $2); }
         | '-' expr %prec MINUS  { $$ = allocUnaryExpression(MINUS_EXPRESSION, $2); }
         | '(' expr ')'          { $$ = $2; }
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
