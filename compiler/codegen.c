#include "summoner.h"
#include <stdlib.h>

typedef struct {
    int label_address;
} LabelTable;

typedef struct {
    int         size;
    int         alloc_size;
    SVM_Byte    *code;
    int         label_table_size;
    int         label_table_alloc_size;
    LabelTable  *label_table;
    int         line_number_size;
    SVM_LineNumber      *line_number;
} OpcodeBuf;

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
init_opcode_buf(OpcodeBuf *ob)
{
    ob->size = 0;
    ob->alloc_size = 0;
    ob->code = NULL;
    ob->label_table_size = 0;
    ob->label_table_alloc_size = 0;
    ob->label_table = NULL;
    ob->line_number_size = 0;
    ob->line_number = NULL;
}

static void
add_global_variable(Compiler *compiler, SVM_Executable *exe)
{
}

static void
add_functions(Compiler *compiler, SVM_Executable *exe)
{
}

// TODO: huge swith-case
static void
generate_statement_list(SVM_Executable *exe, Block *current_block,
                        StatementList *statement_list,
                        OpcodeBuf *ob)
{
}

static void
fix_labels(OpcodeBuf *ob)
{
} 

static SVM_Byte *
fix_opcode_buf(OpcodeBuf *ob)
{
    SVM_Byte *ret;

    fix_labels(ob);
    ret = realloc(ob->code, ob->size);
    free(ob->label_table);

    return ret;
}


static void
add_top_level(Compiler *compiler, SVM_Executable *exe)
{
    OpcodeBuf           ob;

    init_opcode_buf(&ob);
    //TODO: local statement list use - functionDecl -> Block -> statementList
    generate_statement_list(exe, NULL, NULL,
                            &ob);
    
    exe->top_level.code_size = ob.size;
    exe->top_level.code = fix_opcode_buf(&ob);
    exe->top_level.line_number_size = ob.line_number_size;
    exe->top_level.line_number = ob.line_number;
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
