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
    struct StatementList *statement_list;
    struct Block *block;
    struct Elseif *elseif;
}

%token <int_value> BOOL_LITERAL
%token <double_value> DOUBLE_LITERAL
%token <int_value> INT_LITERAL
%token <identifier> IDENTIFIER;
%token FUNCTION IF ELSE FOR RETURN BREAK CONTINUE NIL

%type <expression> expr bool_expr
%type <statement> stmt if_stmt
%type <statement_list> stmt_list
%type <block> block
%type <elseif> elseif elseif_list

%nonassoc '='
%left AND OR
%nonassoc EQ NE
%nonassoc '>' '<' LE GE
%left '+' '-'
%left '*' '/'
%nonassoc MINUS
%nonassoc NOT

%%

stmt_list:
          stmt                {$$ = allocStatementList($1); }
        | stmt_list '\n' stmt {$$ = chainStatementList($1, $3); }
        ;

stmt:
       expr                { printExprValue(evalExpression($1)); }
     | IDENTIFIER '=' expr { $$ = allocAssignStatement($1, $3); }
     | block               { $$ = allocBlockStatement($1); }
     | if_stmt
     ;

if_stmt:
          IF '(' bool_expr ')' block                       { $$ = allocIfStatement($3, $5, NULL, NULL); }
        | IF '(' bool_expr ')' block ELSE block            { $$ = allocIfStatement($3, $5, NULL, $7); } 
        | IF '(' bool_expr ')' block elseif_list           { $$ = allocIfStatement($3, $5, $6, NULL); }
        | IF '(' bool_expr ')' block elseif_list ELSE block { $$ = allocIfStatement($3, $5, $6, $8); }
        ;

elseif_list: 
             elseif
           | elseif_list elseif { $$ = chainElseifList($1, $2); }
           ;

elseif:
       ELSE IF '(' bool_expr ')' block { $$ = allocElseif($4, $6); }
       ;

block:
         '{' stmt_list '}'   { $$ = allocBlock($2); }
       | '{' '}'             { $$ = allocBlock(NULL); }
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
         | '-' expr %prec MINUS  { $$ = allocUnaryExpression(MINUS_EXPRESSION, $2); }
         | '(' expr ')'          { $$ = $2; }
         | bool_expr
         ;

bool_expr:
             expr '>' expr         { $$ = allocBinaryExpression(GT_EXPRESSION, $1, $3); }
           | expr GE expr          { $$ = allocBinaryExpression(GE_EXPRESSION, $1, $3); }
           | expr '<' expr         { $$ = allocBinaryExpression(LT_EXPRESSION, $1, $3); }
           | expr LE expr          { $$ = allocBinaryExpression(LE_EXPRESSION, $1, $3); }
           | expr EQ expr          { $$ = allocBinaryExpression(EQ_EXPRESSION, $1, $3); }
           | expr NE expr          { $$ = allocBinaryExpression(NE_EXPRESSION, $1, $3); }
           | expr AND expr         { $$ = allocBinaryExpression(AND_EXPRESSION, $1, $3); }
           | expr OR expr          { $$ = allocBinaryExpression(OR_EXPRESSION, $1, $3); }
           | '!' expr %prec NOT    { $$ = allocUnaryExpression(NOT_EXPRESSION, $2); }
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
