#include "summoner.h"
#include <stdlib.h>

static Compiler *st_current_compiler;

Compiler *create_compiler()
{
    Compiler *compiler = (Compiler *)malloc(sizeof(Compiler));
    compiler->svm_constant_count = 0;
    compiler->svm_constant = NULL;
    compiler->function_count = 0;
    compiler->func_definition_list = NULL;
    compiler->current_block = NULL;
    compiler->declaration_list = NULL;
    compiler->stmt_list = NULL;
    compiler->current_line_number = 1;
    return compiler;
}

Compiler *get_current_compiler()
{
    return st_current_compiler;
}

void set_current_compiler(Compiler *compiler)
{
    st_current_compiler = compiler;
}

void add_func_definition_to_compiler(FuncDefinition *func_definition)
{
    Compiler *compiler = get_current_compiler();
    compiler->func_definition_list = chain_func_definition_list(compiler->func_definition_list, func_definition);
}

void add_stmt_to_compiler(Statement *stmt)
{
    Compiler *compiler = get_current_compiler();
    compiler->stmt_list = chain_stmt_list(compiler->stmt_list, stmt);
}

void increment_line_number()
{
    get_current_compiler()->current_line_number++;
}
