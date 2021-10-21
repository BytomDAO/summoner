%{
#include <stdio.h>
#include <stdlib.h>
#include "summoner.h"

int yylex();
int yyerror(const char *s);
%}

%union {
    char *                     identifier;
    char *                     str_value;
    double                     double_value;
    int64_t                    int_value;
    struct Expression *        expression;
    struct Declaration *       declaration;
    struct Statement *         statement;
    struct StatementList *     statement_list;
    struct Block *             block;
    struct Elseif *            elseif;
    struct FuncDefinition *     func_definition;
    struct Definition *         definition;
    struct DefinitionList *     definition_list;
    struct TypeSpecifier *      type_specifier;
    struct ParameterList *     parameter_list;
    struct ArgumentList *      argument_list;
    struct StateDeclaration *  state_declaration;
    struct ContractDefinition * contract_definition;
}

%token <double_value> DOUBLE_LITERAL
%token <int_value> INT_LITERAL BOOL_LITERAL N
%token <str_value> STRING_LITERAL
%token <identifier> IDENTIFIER
%token VAR CONST TYPE CONTRACT FUNCTION IF ELSE FOR RETURN BREAK CONTINUE NIL
%token BOOL_T INT_T DOUBLE_T STRING_T ASSET_T HASH_T AMOUNT_T PUBKEY_T SIG_T HEX_T

%type <expression> expr bool_expr func_call_expr identifier_expr literal
%type <declaration> variable_declaration variable_declaration_list
%type <statement> stmt if_stmt return_stmt declaration_stmt assign_stmt compound_assign_stmt variable_declaration_stmt const_stmt
%type <statement_list> stmt_list
%type <block> block
%type <elseif> elseif elseif_list
%type <func_definition> func_definition
%type <definition> definition
%type <definition_list> definition_list
%type <type_specifier> type_specifier
%type <parameter_list> parameter_list parameter
%type <argument_list> argument_list
%type <int_value> new_line
%type <state_declaration> state_declarations state_declaration
%type <contract_definition> contract_definition

%nonassoc '=' ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN DECL_ASSIGN
%left AND OR
%nonassoc EQ NE
%nonassoc '>' '<' LE GE
%left '+' '-'
%left '*' '/'
%nonassoc MINUS
%nonassoc NOT

%%

translation_unit:
      new_line_opt definition_list new_line_opt
    ;

definition_list:
      definition                          { $$ = alloc_definition_list(); }
    | definition_list new_line definition
    ;

definition:
      func_definition             { $$ = alloc_definition(); add_func_definition_to_compiler($1); }
    | contract_definition          { $$ = alloc_definition(); add_contract_definition_to_compiler($1); }
    | const_stmt                  { $$ = alloc_definition(); add_stmt_to_compiler($1); }
    | variable_declaration_stmt   { $$ = alloc_definition(); add_stmt_to_compiler($1); }
    ;

contract_definition:
      TYPE IDENTIFIER CONTRACT '{' new_line_opt '}'                                  { $$ = alloc_contract_definition($2, NULL); }
    | TYPE IDENTIFIER CONTRACT '{' new_line_opt state_declarations new_line_opt '}'  { $$ = alloc_contract_definition($2, $6); }
    ;

state_declarations:
      state_declaration
    | state_declarations new_line state_declaration  { $$ = chain_state_declaration($1, $3); }
    ;

state_declaration:
      IDENTIFIER type_specifier { $$ = alloc_state_declaration($1, $2); }
    ;

func_definition:
      FUNCTION IDENTIFIER '(' parameter_list ')' type_specifier block  { $$ = alloc_func_definition($2, $4, $6, $7); }
    | FUNCTION IDENTIFIER '(' parameter_list ')' block                 { $$ = alloc_func_definition($2, $4, NULL, $6); }
    ;

parameter_list:
                                    { $$ = NULL; }
    | parameter
    | parameter_list ',' parameter  { $$ = chain_parameter($1, $3); }
    ;

parameter:
      IDENTIFIER type_specifier  { $$ = alloc_parameter($2, $1); }
    ;

stmt_list:
      stmt                     { $$ = chain_stmt_list(NULL, $1); }
    | stmt_list new_line stmt  { $$ = chain_stmt_list($1, $3); }
    ;

stmt:
      expr               { $$ = alloc_expression_stmt($1); }
    | block              { $$ = alloc_block_stmt($1); }
    | assign_stmt
    | if_stmt
    | declaration_stmt
    | return_stmt
    ;

assign_stmt:
      identifier_expr '=' expr  { $$ = alloc_assign_stmt($1, $3); }
    | compound_assign_stmt
    ;

compound_assign_stmt:
      identifier_expr ADD_ASSIGN expr  { $$ = alloc_compound_assign_stmt($1, ADD_EXPRESSION, $3); }
    | identifier_expr SUB_ASSIGN expr  { $$ = alloc_compound_assign_stmt($1, SUB_EXPRESSION, $3); }
    | identifier_expr MUL_ASSIGN expr  { $$ = alloc_compound_assign_stmt($1, MUL_EXPRESSION, $3); }
    | identifier_expr DIV_ASSIGN expr  { $$ = alloc_compound_assign_stmt($1, DIV_EXPRESSION, $3); }
    ;

const_stmt:
      CONST IDENTIFIER '=' expr                 { $$ = alloc_const_declaration_stmt($2, NULL, $4); }
    | CONST IDENTIFIER type_specifier '=' expr  { $$ = alloc_const_declaration_stmt($2, $3, $5); }
    ;

declaration_stmt:
      IDENTIFIER DECL_ASSIGN expr  { $$ = alloc_declaration_stmt(alloc_declaration($1, NULL, $3)); }
    | variable_declaration_stmt
    ;

variable_declaration_stmt:
      VAR new_line_opt variable_declaration                            { $$ = alloc_declaration_stmt($3); }
    | VAR '(' new_line_opt variable_declaration_list new_line_opt ')'  { $$ = alloc_declaration_stmt($4); }
    ;

variable_declaration_list:
      variable_declaration
    | variable_declaration_list new_line variable_declaration  { $$ = chain_declaration_list($1, $3); }
    ;

variable_declaration:       
      IDENTIFIER type_specifier           { $$ = alloc_declaration($1, $2, NULL); }
    | IDENTIFIER type_specifier '=' expr  { $$ = alloc_declaration($1, $2, $4); }
    | IDENTIFIER '=' expr                 { $$ = alloc_declaration($1, NULL, $3); }
    ;

return_stmt:
      RETURN expr  { $$ = alloc_return_stmt($2); }
    ;

if_stmt:
      IF bool_expr block elseif_list             { $$ = alloc_if_stmt($2, $3, $4, NULL); }
    | IF bool_expr block elseif_list ELSE block  { $$ = alloc_if_stmt($2, $3, $4, $6); }
    ;

elseif_list:
                          { $$ = NULL; }
    | elseif_list elseif  { $$ = chain_else_if_list($1, $2); }
    ;

elseif:
      ELSE IF bool_expr block  { $$ = alloc_else_if($3, $4); }
    ;

type_specifier:
      BOOL_T    { $$ = alloc_type_specifier(BOOLEAN_TYPE); }
    | INT_T     { $$ = alloc_type_specifier(INT_TYPE); }
    | DOUBLE_T  { $$ = alloc_type_specifier(DOUBLE_TYPE); }
    | STRING_T  { $$ = alloc_type_specifier(STRING_TYPE); }
    | ASSET_T   { $$ = alloc_type_specifier(ASSET_TYPE); }
    | HASH_T    { $$ = alloc_type_specifier(HASH_TYPE); }
    | AMOUNT_T  { $$ = alloc_type_specifier(AMOUNT_TYPE); }
    | PUBKEY_T  { $$ = alloc_type_specifier(PUBKEY_TYPE); }
    | SIG_T     { $$ = alloc_type_specifier(SIG_TYPE); }
    | HEX_T     { $$ = alloc_type_specifier(HEX_TYPE); }
    ;

block:
      '{' new_line_opt                             { $<block>$ = open_block(); }
                      stmt_list new_line_opt  '}'  { $$ = close_block($<block>3, $4); }
    | '{' new_line_opt '}'                         { $<block>$ = alloc_block(NULL); }
    ;

expr:
      literal
    | identifier_expr
    | expr '+' expr                { $$ = alloc_binary_expression(ADD_EXPRESSION, $1, $3); }
    | expr '-' expr                { $$ = alloc_binary_expression(SUB_EXPRESSION, $1, $3); }
    | expr '*' expr                { $$ = alloc_binary_expression(MUL_EXPRESSION, $1, $3); }
    | expr '/' expr                { $$ = alloc_binary_expression(DIV_EXPRESSION, $1, $3); }
    | '-' expr %prec MINUS         { $$ = alloc_unary_expression(MINUS_EXPRESSION, $2); }
    | '(' expr ')'                 { $$ = $2; }
    | type_specifier '(' expr ')'  { $$ = alloc_type_cast_expression($1, $3); }
    | bool_expr
    | func_call_expr
    ;

literal:
      INT_LITERAL     { $$ = alloc_int_expression($1); }
    | DOUBLE_LITERAL  { $$ = alloc_double_expression($1); }
    | BOOL_LITERAL    { $$ = alloc_bool_expression($1); }
    | STRING_LITERAL  { $$ = alloc_string_expression($1); }
    ;

bool_expr:
      expr '>' expr       { $$ = alloc_binary_expression(GT_EXPRESSION, $1, $3); }
    | expr GE expr        { $$ = alloc_binary_expression(GE_EXPRESSION, $1, $3); }
    | expr '<' expr       { $$ = alloc_binary_expression(LT_EXPRESSION, $1, $3); }
    | expr LE expr        { $$ = alloc_binary_expression(LE_EXPRESSION, $1, $3); }
    | expr EQ expr        { $$ = alloc_binary_expression(EQ_EXPRESSION, $1, $3); }
    | expr NE expr        { $$ = alloc_binary_expression(NE_EXPRESSION, $1, $3); }
    | expr AND expr       { $$ = alloc_binary_expression(AND_EXPRESSION, $1, $3); }
    | expr OR expr        { $$ = alloc_binary_expression(OR_EXPRESSION, $1, $3); }
    | '!' expr %prec NOT  { $$ = alloc_unary_expression(NOT_EXPRESSION, $2); }
    ;

func_call_expr:
      identifier_expr '(' argument_list ')'  { $$ = alloc_func_call_expression($1, $3); }
    ;

identifier_expr:
      IDENTIFIER  { $$ = alloc_identifier_expression($1); }
    ;

argument_list:
                             { $$ = NULL; }
    | expr                   { $$ = chain_argument_list(NULL, $1); }
    | argument_list ',' expr { $$ = chain_argument_list($1, $3); }
    ;

new_line_opt:
    | new_line
    ;

new_line:
      N           { $$ = $1; increment_line_number(); }
    | new_line N  { $$ = $1; increment_line_number(); }
    ;

%%
