%{
#include <stdio.h>
#include <stdlib.h>
#include "compiler/summoner.h"

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
    struct TypeSpecifier *type_specifier;
    struct ParameterList *parameter_list;
    struct Definition *definition;
    struct DefinitionList *definition_list;
    struct ArgumentList *argument_list;
}

%token <int_value> BOOL_LITERAL
%token <double_value> DOUBLE_LITERAL
%token <int_value> INT_LITERAL
%token <identifier> IDENTIFIER;
%token VAR FUNCTION IF ELSE FOR RETURN BREAK CONTINUE NIL
%token BOOL_T INT_T DOUBLE_T STRING_T

%type <expression> expr bool_expr func_call_expr
%type <statement> stmt if_stmt declaration_stmt return_stmt variable_declaration
%type <statement_list> stmt_list
%type <block> block
%type <elseif> elseif elseif_list
%type <type_specifier> type_specifier
%type <parameter_list> parameter_list paramter
%type <definition> definition func_definition
%type <definition_list> definition_list
%type <argument_list> argument_list

%nonassoc '=' DECL_ASSIGN
%left AND OR
%nonassoc EQ NE
%nonassoc '>' '<' LE GE
%left '+' '-'
%left '*' '/'
%nonassoc MINUS
%nonassoc NOT

%%

translation_unit:
                   definition_list                   { add_definitions_to_compiler($1); }
                |  definition_list new_line          { add_definitions_to_compiler($1); }
                |  new_line definition_list          { add_definitions_to_compiler($2); }
                |  new_line definition_list new_line { add_definitions_to_compiler($2); }
                ;

definition_list:
                  definition                          { $$ = alloc_definition_list($1); }
                | definition_list new_line definition { $$ = chain_definition_list($1, $3); }
                ;

definition:
            func_definition
          | variable_declaration { $$ = alloc_declaration_definition($1); }
          ;

func_definition:
                 FUNCTION IDENTIFIER '(' parameter_list ')' type_specifier block { $$ = alloc_func_definition($2, $4, $6, $7); }
               | FUNCTION IDENTIFIER '(' ')' type_specifier block                { $$ = alloc_func_definition($2, NULL, $5, $6); }
               | FUNCTION IDENTIFIER '(' parameter_list ')' block                { $$ = alloc_func_definition($2, $4, NULL, $6); }
               | FUNCTION IDENTIFIER '(' ')' block                               { $$ = alloc_func_definition($2, NULL, NULL, $5); }
               ;

parameter_list:
                 paramter
               | parameter_list ',' paramter { $$ = chain_parameter($1, $3); }
               ;

paramter:
         IDENTIFIER type_specifier { $$ = alloc_parameter($2, $1); }
         ;

stmt_list:
          stmt                    {$$ = chain_stmt_list(NULL, $1); }
        | stmt_list new_line stmt {$$ = chain_stmt_list($1, $3); }
        ;

stmt:
       IDENTIFIER '=' expr { $$ = alloc_assign_stmt($1, $3); }
     | expr                { $$ = alloc_expression_stmt($1); }
     | block               { $$ = alloc_block_stmt($1); }
     | if_stmt
     | declaration_stmt
     | return_stmt
     ;

declaration_stmt:
                    IDENTIFIER DECL_ASSIGN expr            { $$ = alloc_declaration_stmt($1, NULL, $3); }
                  | variable_declaration  
                  ;

variable_declaration:
                        VAR IDENTIFIER type_specifier          { $$ = alloc_declaration_stmt($2, $3, NULL); }
                      | VAR IDENTIFIER type_specifier '=' expr { $$ = alloc_declaration_stmt($2, $3, $5); }
                      | VAR IDENTIFIER '=' expr                { $$ = alloc_declaration_stmt($2, NULL, $4); }
                      ;

type_specifier:
                BOOL_T   { $$ = alloc_type_specifier(BOOLEAN_TYPE, NULL); }
              | INT_T    { $$ = alloc_type_specifier(INT_TYPE, NULL); }
              | DOUBLE_T { $$ = alloc_type_specifier(DOUBLE_TYPE, NULL); }
              | STRING_T { $$ = alloc_type_specifier(STRING_TYPE, NULL); }
              ;

return_stmt:
             RETURN expr { $$ = alloc_return_stmt($2); }
           ;

if_stmt:
          IF bool_expr block                        { $$ = alloc_if_stmt($2, $3, NULL, NULL); }
        | IF bool_expr block ELSE block             { $$ = alloc_if_stmt($2, $3, NULL, $5); } 
        | IF bool_expr block elseif_list            { $$ = alloc_if_stmt($2, $3, $4, NULL); }
        | IF bool_expr block elseif_list ELSE block { $$ = alloc_if_stmt($2, $3, $4, $6); }
        ;

elseif_list: 
             elseif
           | elseif_list elseif { $$ = chain_else_if_list($1, $2); }
           ;

elseif:
       ELSE IF bool_expr block { $$ = alloc_else_if($3, $4); }
       ;

block:
         '{' stmt_list                         { $<block>$ = open_block(); }
                       '}'                     { $$ = close_block($<block>3, $2); }
       | '{' new_line stmt_list                { $<block>$ = open_block(); }
                                '}'            { $$ = close_block($<block>4, $3); }
       | '{' stmt_list new_line                { $<block>$ = open_block(); }
                                '}'            { $$ = close_block($<block>4, $2); }
       | '{' new_line stmt_list new_line       { $<block>$ = open_block(); }
                                         '}'   { $$ = close_block($<block>5, $3); }
       | '{' new_line '}'                      { $<block>$ = alloc_block(NULL); }
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
         | func_call_expr
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

func_call_expr:
                IDENTIFIER '(' ')'               { $$ = alloc_func_call_expression($1, NULL); }
              | IDENTIFIER '(' argument_list ')' { $$ = alloc_func_call_expression($1, $3); }
              ;

argument_list:
               expr                   { $$ = chain_argument_list(NULL, $1); }
             | argument_list ',' expr { $$ = chain_argument_list($1, $3); }
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
