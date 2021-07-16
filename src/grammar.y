%{
#include <stdio.h>
#include <stdlib.h>

int yylex();
int yyerror(const char *s);
%}

%union {
    double double_value;
}

%token <double_value> DOUBLE_LITERAL
%token CR LP RP
%left ADD SUB
%left MUL DIV

%type <double_value> expr

%%
stmt_list:
          stmt
        | stmt_list stmt
        ;

stmt:
     expr CR {
         printf(">>%lf\n", $1);
     }

expr:
           DOUBLE_LITERAL { $$ = $1; }
         | expr ADD expr  { $$ = $1 + $3; }
         | expr SUB expr  { $$ = $1 - $3; }
         | expr MUL expr  { $$ = $1 * $3; }
         | expr DIV expr  { $$ = $1 / $3; }
         | LP expr RP     { $$ = $2; }
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
