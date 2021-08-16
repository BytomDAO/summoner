#include <stdio.h>
#include <stdlib.h>
#include "../compiler/summoner.h"

int main(int argc, char *argv[])
{
    SVM_Executable *exe;

    Compiler *compiler = create_compiler();
    set_current_compiler(compiler);

    extern int yyparse();
    extern FILE *yyin;
    yyin = fopen(argv[1], "r");
    if (yyin == NULL)
    {
        printf("fail to open file:%s\n", argv[1]);
        exit(1);
    }

    yyparse();
    fix_tree(compiler);
    exe = smc_code_gen(compiler);
    svm_disassemble(exe);

    return 0;
}
