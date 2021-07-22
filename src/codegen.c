#include "summoner.h"
#include <stdlib.h>

static SVM_Executable *
alloc_executable()
{
    SVM_Executable      *exe;

    exe = malloc(sizeof(SVM_Executable));

    exe->constant_pool_count = 0;
    exe->constant_pool = NULL;
    exe->global_variable_count = 0;
    exe->global_variable = NULL;
    exe->constant_count = 0;
    exe->constant_definition = NULL;
    exe->type_specifier_count = 0;
    exe->type_specifier = NULL;

    return exe;
}

static void
add_global_variable(Compiler *compiler, SVM_Executable *exe)
{
}

static void
add_functions(Compiler *compiler, SVM_Executable *exe)
{
}

static void
add_top_level(Compiler *compiler, SVM_Executable *exe)
{
}
static void
generate_constant_initializer(Compiler *compiler, SVM_Executable *exe)
{
}

SVM_Executable *
smc_code_gen(Compiler *compiler)
{
    SVM_Executable      *exe;

    exe = alloc_executable();

    exe->constant_count = compiler->svm_constant_count;
    exe->constant_definition = compiler->svm_constant;

    add_global_variable(compiler, exe);
    add_functions(compiler, exe);
    add_top_level(compiler, exe);

    generate_constant_initializer(compiler, exe);

    return exe;
}
