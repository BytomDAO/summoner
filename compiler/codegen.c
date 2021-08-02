#include "summoner.h"
#include <stdlib.h>
#include <string.h>

typedef struct
{
    int label_address;
} LabelTable;

typedef struct
{
    int size;
    int alloc_size;
    SVM_Byte *code;
    int label_table_size;
    int label_table_alloc_size;
    LabelTable *label_table;
    int line_number_size;
    SVM_LineNumber *line_number;
} OpcodeBuf;

static SVM_Executable *
alloc_executable()
{
    SVM_Executable *exe;

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
    Declaration *dl;
    int i, var_count = 0;

    for (dl = compiler->declaration_list; dl; dl = dl->next)
    {
        var_count++;
    }
    exe->global_variable_count = var_count;
    exe->global_variable = (SVM_Variable *)malloc(sizeof(SVM_Variable) * var_count);

    for (dl = compiler->declaration_list, i = 0; dl; dl = dl->next, i++)
    {
        strcpy(exe->global_variable[i].name, dl->name);
        exe->global_variable[i].type = dl->type;
    }
    // TODO: derive type: FUNCTION_DERIVE/ARRAY_DERIVE
}

static void
generate_statement_list(SVM_Executable *exe, Block *current_block,
                        StatementList *statement_list,
                        OpcodeBuf *ob);

static void
add_function(SVM_Executable *exe, FuncDefinition *src)
{
    OpcodeBuf ob;
    init_opcode_buf(&ob);
    generate_statement_list(exe, src->block, src->block->statement_list, &ob);
}

static void
add_functions(Compiler *compiler, SVM_Executable *exe)
{
    FuncDefinition *fd;
    for (fd = compiler->func_definition_list; fd; fd = fd->next)
    {
        add_function(exe, fd);
    }
}

static void
generate_assign_statement(SVM_Executable *exe, Block *current_block,
                          AssignStatement *assign_stmt,
                          OpcodeBuf *ob)
{
}

static void
generate_block_statement(SVM_Executable *exe, Block *current_block,
                         Block *block_stmt,
                         OpcodeBuf *ob)
{
}

static void
generate_if_statement(SVM_Executable *exe, Block *current_block,
                      IfStatement *if_smt,
                      OpcodeBuf *ob)
{
}

static void
generate_return_statement(SVM_Executable *exe, Block *current_block,
                          Expression *return_stmt,
                          OpcodeBuf *ob)
{
}

static void
generate_initializer(SVM_Executable *exe, Block *current_block,
                     Declaration *decl_stmt,
                     OpcodeBuf *ob)
{
}

static void
generate_expression_statement(SVM_Executable *exe, Block *current_block,
                              Expression *expr,
                              OpcodeBuf *ob)
{
}

// TODO: huge swith-case
static void
generate_statement_list(SVM_Executable *exe, Block *current_block,
                        StatementList *statement_list,
                        OpcodeBuf *ob)
{
    StatementList *pos;

    for (pos = statement_list; pos; pos = pos->next)
    {
        switch (pos->statement->kind)
        {
        case ASSIGN_STATEMENT:
            generate_assign_statement(exe, current_block, pos->statement->u.assign_s, ob);
        case BLOCK_STATEMENT:
            generate_block_statement(exe, current_block, pos->statement->u.block_s, ob);
        case IF_STATEMENT:
            generate_if_statement(exe, current_block, pos->statement->u.if_s, ob);
            break;
        case RETURN_STATEMENT:
            generate_return_statement(exe, current_block, pos->statement->u.expr_s, ob);
            break;
        case DECLARATION_STATEMENT:
            generate_initializer(exe, current_block,
                                 pos->statement->u.decl_s, ob);
            break;
        case EXPRESSION_STATEMENT:
            generate_expression_statement(exe, current_block,
                                          pos->statement->u.expr_s, ob);
            break;
        default:
            printf("pos->statement->kind..%d\n", pos->statement->kind);
            exit(1);
            // TODO: use assert micro
            // DBG_assert(0, ("pos->statement->kind..%d\n", pos->statement->kind));
        }
    }
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
    OpcodeBuf ob;

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
    SVM_Executable *exe;

    exe = alloc_executable();

    exe->constant_count = compiler->svm_constant_count;
    exe->constant_definition = compiler->svm_constant;

    add_global_variable(compiler, exe);
    add_functions(compiler, exe);
    add_top_level(compiler, exe);

    generate_constant_initializer(compiler, exe);

    return exe;
}
