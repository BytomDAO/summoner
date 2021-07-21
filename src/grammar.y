%{
#include <stdio.h>
#include <stdlib.h>
#include "src/ast.h"
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
          stmt                    {$$ = alloc_statement_list($1); }
        | stmt_list new_line stmt {$$ = chain_statement_list($1, $3); }
        ;

stmt:
       expr                { printExprValue(evalExpression($1)); }
     | IDENTIFIER '=' expr { $$ = alloc_assign_statement($1, $3); }
     | block               { $$ = alloc_block_statement($1); }
     | if_stmt
     ;

if_stmt:
          IF bool_expr block                        { $$ = alloc_if_statement($2, $3, NULL, NULL); }
        | IF bool_expr block ELSE block             { $$ = alloc_if_statement($2, $3, NULL, $5); } 
        | IF bool_expr block elseif_list            { $$ = alloc_if_statement($2, $3, $4, NULL); }
        | IF bool_expr block elseif_list ELSE block { $$ = alloc_if_statement($2, $3, $4, $6); }
        ;

elseif_list: 
             elseif
           | elseif_list elseif { $$ = chain_else_if_list($1, $2); }
           ;

elseif:
       ELSE IF bool_expr block { $$ = alloc_else_if($3, $4); }
       ;

block:
         '{' stmt_list '}'                     { $$ = alloc_block($2); }
       | '{' new_line stmt_list '}'            { $$ = alloc_block($3); }
       | '{' stmt_list new_line '}'            { $$ = alloc_block($2); }
       | '{' new_line stmt_list new_line '}'   { $$ = alloc_block($3); }
       | '{' new_line '}'                      { $$ = alloc_block(NULL); }
       | '{' '}'                               { $$ = alloc_block(NULL); }
     ;

expr:
           INT_LITERAL           { $$ = alloc_int_expression($1); }
         | DOUBLE_LITERAL        { $$ = alloc_double_expression($1); }
         | BOOL_LITERAL          { $$ = alloc_bool_expression($1); }
         | IDENTIFIER            { $$ = alloc_identifier_expression($1); }
         | expr '+' expr         { $$ = alloc_binary_expression(ADD_EXPRESSION, $1, $3); }
         | expr '-' expr         { $$ = alloc_binary_expression(SUB_EXPRESSION, $1, $3); }
         | expr '*' expr         { $$ = alloc_binary_expression(MUL_EXPRESSION, $1, $3); }
         | expr '/' expr         { $$ = alloc_binary_expression(DIV_EXPRESSION, $1, $3); }
         | '-' expr %prec MINUS  { $$ = alloc_unary_expression(MINUS_EXPRESSION, $2); }
         | '(' expr ')'          { $$ = $2; }
         | bool_expr
         ;

bool_expr:
             expr '>' expr         { $$ = alloc_binary_expression(GT_EXPRESSION, $1, $3); }
           | expr GE expr          { $$ = alloc_binary_expression(GE_EXPRESSION, $1, $3); }
           | expr '<' expr         { $$ = alloc_binary_expression(LT_EXPRESSION, $1, $3); }
           | expr LE expr          { $$ = alloc_binary_expression(LE_EXPRESSION, $1, $3); }
           | expr EQ expr          { $$ = alloc_binary_expression(EQ_EXPRESSION, $1, $3); }
           | expr NE expr          { $$ = alloc_binary_expression(NE_EXPRESSION, $1, $3); }
           | expr AND expr         { $$ = alloc_binary_expression(AND_EXPRESSION, $1, $3); }
           | expr OR expr          { $$ = alloc_binary_expression(OR_EXPRESSION, $1, $3); }
           | '!' expr %prec NOT    { $$ = alloc_unary_expression(NOT_EXPRESSION, $2); }
           ;

new_line:
           '\n'
         | new_line '\n'

%%

int yyerror(char const *str) {
    extern char *yytext;
    fprintf(stderr, "parse error near %s\n", yytext);
    return 0;
}

int main(int argc, char *argv[]) {
    extern FILE *yyin;
    yyin = fopen(argv[1], "r");
    if(yyin==NULL)
    {
        printf("fail to open file:%s\n", argv[1]);
    } else 
    {
        yyparse();
    }
    return 0;
}
