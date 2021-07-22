#include <stdio.h>
#include <stdlib.h>
#include "../src/summoner.h"

int main(int argc, char *argv[])
{
    Compiler *compiler = malloc(sizeof(Compiler));
    set_current_compiler(compiler);

    extern int yyparse();
    extern FILE *yyin;
    yyin = fopen(argv[1], "r");
    if (yyin == NULL)
    {
        printf("fail to open file:%s\n", argv[1]);
    }
    else
    {
        yyparse();
    }
    return 0;
}