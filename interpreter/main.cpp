#include <iostream>
#include "eval.h"
#include "../compiler/summoner.h"

using namespace std;

int main(int argc, char *argv[])
{
    Compiler *compiler = new Compiler();
    set_current_compiler(compiler);

    extern int yyparse();
    extern FILE *yyin;
    yyin = fopen(argv[1], "r");
    if (yyin == NULL)
    {
        cout << "fail to open file:" << argv[1] << endl;
        return 1;
    }
    else
    {
        yyparse();
    }

    Interpreter *interpreter = new Interpreter(compiler->func_definition_list, compiler->declaration_list);
    interpreter->exec();
    return 0;
}
