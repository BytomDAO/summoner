#include "summoner.h"
#include <stdlib.h>
#include <string.h>
#include "../include/SVM_code.h"

extern OpcodeInfo svm_opcode_info[];

#define OPCODE_ALLOC_SIZE (256)
#define LABEL_TABLE_ALLOC_SIZE (256)

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
    exe->function_count = 0;
    exe->function = NULL;
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
fix_labels(OpcodeBuf *ob)
{
   int i;
    int j;
    OpcodeInfo *info;
    int label;
    int address;

    for (i = 0; i < ob->size; i++) {
        if (ob->code[i] == OP_JUMP
            || ob->code[i] == OP_JUMPIF) {
            label = (ob->code[i+1] << 8) + (ob->code[i+2]);
            address = ob->label_table[label].label_address;
            ob->code[i+1] = (SVM_Byte)(address >> 8);
            ob->code[i+2] = (SVM_Byte)(address &0xff);
        }
        info = &svm_opcode_info[ob->code[i]];
        for (j = 0; info->parameter[j] != '\0'; j++) {
            switch (info->parameter[j]) {
            case 'b':
                i++;
                break;
            case 's':
            case 'p':
                i += 2;
                break;
            default:
                printf("param..%s, j..%d", info->parameter, j);
                exit(1);
            }
        }
    }
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
generate_builtin_code(OpcodeBuf *ob, SVM_Opcode *ops, int op_cnt)
{
    for(int i = 0; i< op_cnt; i++) {
        ob->code[ob->size] = ops[i];
        ob->size++;
    }
}

static void
generate_code(OpcodeBuf *ob, SVM_Opcode code,  ...)
{
    va_list     ap;
    int         i;
    char        *param;
    int         param_count;
    int         start_pc;

    va_start(ap, code);

    param = svm_opcode_info[(int)code].parameter;
    param_count = strlen(param);
    if (ob->alloc_size < ob->size + 1 + (param_count * 2)) {
        ob->code = realloc(ob->code, ob->alloc_size + OPCODE_ALLOC_SIZE);
        ob->alloc_size += OPCODE_ALLOC_SIZE;
    }

    start_pc = ob->size;
    ob->code[ob->size] = code;
    ob->size++;
    for (i = 0; param[i] != '\0'; i++) {
        unsigned int value = va_arg(ap, int);
        switch (param[i]) {
        case 'b': /* byte */
            ob->code[ob->size] = (SVM_Byte)value;
            ob->size++;
            break;
        case 's': /* short(2byte int) */
            ob->code[ob->size] = (SVM_Byte)(value >> 8);
            ob->code[ob->size+1] = (SVM_Byte)(value & 0xff);
            ob->size += 2;
            break;
        case 'p': /* constant pool index */
            ob->code[ob->size] = (SVM_Byte)(value >> 8);
            ob->code[ob->size+1] = (SVM_Byte)(value & 0xff);
            ob->size += 2;
            break;
        default:
            printf("param..%s, i..%d", param, i);
            exit(1);
        }
    }

    va_end(ap);
}

static void
generate_statement_list(SVM_Executable *exe, Block *current_block,
                        StatementList *statement_list,
                        OpcodeBuf *ob);

static void generate_expression(SVM_Executable *exe, Block *current_block,
                                Expression *expr, OpcodeBuf *ob);

/*
// TODO: LocalVariable for FuncDefinition
static SVM_Variable *
copy_local_variables(FuncDefinition *fd, int param_count)
{
    int i;
    int local_variable_count;
    SVM_Variable *dest;

    local_variable_count = fd->local_variable_count - param_count;

    dest = malloc(sizeof(SVM_LocalVariable) * local_variable_count);

    for (i = 0; i < local_variable_count; i++) {
        dest[i].name
            = strdup(fd->local_variable[i+param_count]->name);
        dest[i].type
            = copy_type_specifier(fd->local_variable[i+param_count]->type);
    }

    return dest;
}
*/

static int
count_parameter(ParameterList *src)
{
    ParameterList *param;
    int param_count = 0;

    for (param = src; param; param = param->next) {
        param_count++;
    }

    return param_count;
}

static SVM_Variable *
copy_parameter_list(ParameterList *src, int *param_count_p)
{
    ParameterList *param;
    SVM_Variable *dest;
    int param_count;
    int i;

    param_count = count_parameter(src);
    *param_count_p = param_count;

    dest = malloc(sizeof(SVM_Variable) * param_count);
    
    for (param = src, i = 0; param; param = param->next, i++) {
        dest[i].name = strdup(param->name);
        dest[i].type = alloc_type_specifier(param->type->basic_type);
    }

    return dest;
}
      
static void
add_function(SVM_Executable *exe, FuncDefinition *src, SVM_Function *dest)
{
    OpcodeBuf ob;
    init_opcode_buf(&ob);

    dest->type = alloc_type_specifier(src->return_type->basic_type);
    dest->parameter = copy_parameter_list(src->parameters,
                                          &dest->parameter_count);

    generate_statement_list(exe, src->code_block, src->code_block->statement_list, &ob);
    dest->code_block.code_size = ob.size;
    dest->code_block.code = fix_opcode_buf(&ob);
    dest->code_block.line_number_size = ob.line_number_size;
    dest->code_block.line_number = ob.line_number;
    //dest->local_variable
    //    = copy_local_variables(src, dest->parameter_count);
    // dest->local_variable
    //    = src->local_variable_count - dest->parameter_count;
}

static void
add_functions(Compiler *compiler, SVM_Executable *exe)
{
    FuncDefinition *fd;
    for (fd = compiler->func_definition_list; fd; fd = fd->next)
    {
        add_function(exe, fd, &compiler->svm_function[0]);
    }
}

static void
generate_pop_to_identifier(Declaration *decl, OpcodeBuf *ob)
{
    switch (decl->variable_index) {
    case 0:
        break;
    case 1:
        generate_code(ob, OP_DUP);
        break;    
    default:
        generate_code(ob, OP_ROLL, decl->variable_index);
        break;
    }
    generate_code(ob, OP_DROP);
}

static void
generate_pop_to_lvalue(SVM_Executable *exe, Block *block,
                       Expression *expr, OpcodeBuf *ob)
{
    generate_pop_to_identifier(expr->u.identifier->u.declaration, ob);

}
static void
generate_assign_statement(SVM_Executable *exe, Block *block,
                          AssignStatement *assign_stmt,
                          OpcodeBuf *ob)
{
    generate_expression(exe, block, assign_stmt->operand, ob);
    generate_pop_to_lvalue(exe, block, assign_stmt->left, ob);

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
generate_return_statement(SVM_Executable *exe, Block *block,
                          Expression *return_stmt,
                          OpcodeBuf *ob)
{
    generate_expression(exe, block, return_stmt, ob);
}

static void
generate_initializer(SVM_Executable *exe, Block *current_block,
                     Declaration *decl_stmt,
                     OpcodeBuf *ob)
{
    generate_pop_to_identifier(decl_stmt, ob);
}

static void
generate_boolean_expression(SVM_Executable *cf, Expression *expr,
                            OpcodeBuf *ob)
{
    if (expr->u.boolean_value) {
        generate_code(ob, OP_TRUE, 1);
    } else {
        generate_code(ob, OP_FALSE, 0);
    }
}

static void
generate_int_expression(SVM_Executable *cf, int value,
                        OpcodeBuf *ob)
{
    if (value > 0 && value < 16) {
        generate_code(ob, OP_1 + value - 1, value);
    } else if (value >= 0 && value < 65536) {
        generate_code(ob, OP_PUSHDATA2, value);
    } else {
    // TODO: add constpool
    }
}

// NOTE: no bvm ops for double type
static void
generate_double_expression(SVM_Executable *cf, Expression *expr,
                           OpcodeBuf *ob)
{
}

static void
generate_identifier(IdentifierExpression *identifier_expr, OpcodeBuf *ob)
{
}

static void
generate_identifier_expression(SVM_Executable *exe, Block *block,
                               Expression *expr, OpcodeBuf *ob)
{
    switch (expr->kind) {
    case DECLARATION_DEFINITION:
        generate_identifier(expr->u.identifier, ob);
        break;
    case FUNC_DEFINITION:
        generate_code(ob, OP_INVOKE);
        break;
    case CONST_DEFINITION:
        generate_identifier(expr->u.identifier, ob);
        break;
    case STRUCT_DEFINITION:
        break;
    case IDENTIFIER_EXPRESSION:
        generate_pop_to_lvalue(exe, block, expr, ob);
        break;
    default:
        printf("bad default. kind..%d", expr->kind);
        exit(1);
    }
}

static void
generate_push_argument(SVM_Executable *exe, Block *block,
                       ArgumentList *arg_list, OpcodeBuf *ob)
{
    ArgumentList *arg_pos;

    for (arg_pos = arg_list; arg_pos; arg_pos = arg_pos->next) {
        generate_expression(exe, block, arg_pos->expr, ob);
    }
}

static void
generate_function_call_expression(SVM_Executable *exe, Block *block,
                                  Expression *expr, OpcodeBuf *ob)
{
    FuncCallExpression *fce = expr->u.func_call_expression;
    generate_push_argument(exe, block, fce->argument_list, ob);
    generate_expression(exe, block, fce->argument_list->expr, ob);

    BuiltinFun *builtin_fun = expr->u.func_call_expression
                                ->function->u.identifier->u.builtin_func;
    generate_builtin_code(ob, builtin_fun->op_codes, builtin_fun->ops_count);
}

static void
generate_binary_expression(SVM_Executable *exe, Block *block,
                           Expression *expr, SVM_Opcode code,
                           OpcodeBuf *ob)
{
    int offset;
    Expression *left = expr->u.binary_expression->left;
    Expression *right = expr->u.binary_expression->right;

    generate_expression(exe, block, left, ob);
    generate_expression(exe, block, right, ob);
    // TODO : caculate offset
    // offset = get_binary_expression_offset(left, right, code);

    generate_code(ob, code);
}

static int
get_label(OpcodeBuf *ob)
{
    int ret;

    if (ob->label_table_alloc_size < ob->label_table_size + 1) {
        ob->label_table = realloc(ob->label_table,
                                    (ob->label_table_alloc_size
                                    + LABEL_TABLE_ALLOC_SIZE)
                                    * sizeof(LabelTable));
        ob->label_table_alloc_size += LABEL_TABLE_ALLOC_SIZE;
    }
    ret = ob->label_table_size;
    ob->label_table_size++;

    return ret;
}

static void
set_label(OpcodeBuf *ob, int label)
{
    ob->label_table[label].label_address = ob->size;
}

static void
generate_logical_expression(SVM_Executable *exe, Block *block,
                               Expression *expr, OpcodeBuf *ob,
                               SVM_Opcode code)
{
    int true_label;

    true_label = get_label(ob);
    generate_expression(exe, block, expr->u.binary_expression->left, ob);
    generate_code(ob, OP_DUP);
    generate_code(ob, OP_JUMPIF, true_label);
    generate_expression(exe, block, expr->u.binary_expression->right, ob);
    generate_code(ob, code);
    set_label(ob, true_label);
}


static void
generate_expression(SVM_Executable *exe, Block *current_block,
                    Expression *expr, OpcodeBuf *ob)
{
    switch (expr->kind) {
    case BOOL_EXPRESSION:
        generate_boolean_expression(exe, expr, ob);
        break;
    case INT_EXPRESSION:
        generate_int_expression(exe, expr->u.int_value,
                                ob);
        break;
    case DOUBLE_EXPRESSION:
        generate_double_expression(exe, expr, ob);
        break;
    case IDENTIFIER_EXPRESSION:
        generate_identifier_expression(exe, current_block,
                                       expr, ob);
        break;
   case FUNC_CALL_EXPRESSION:
        generate_function_call_expression(exe, current_block,
                                          expr, ob);
        break;
   case ADD_EXPRESSION:
        generate_binary_expression(exe, current_block, expr,
                                   OP_ADD, ob);
        break;
    case SUB_EXPRESSION:
        generate_binary_expression(exe, current_block, expr,
                                   OP_SUB, ob);
        break;
    case MUL_EXPRESSION:
        generate_binary_expression(exe, current_block, expr,
                                   OP_MUL, ob);
        break;
    case DIV_EXPRESSION:
        generate_binary_expression(exe, current_block, expr,
                                   OP_DIV, ob);
        break;
    case MOD_EXPRESSION:
        generate_binary_expression(exe, current_block, expr,
                                   OP_MOD, ob);
        break;
    case EQ_EXPRESSION:
        generate_binary_expression(exe, current_block, expr,
                                   OP_EQUAL, ob);
        break;
    case NE_EXPRESSION:
        generate_binary_expression(exe, current_block, expr,
                                   OP_0NOTEQUAL, ob);
        break;
    case GT_EXPRESSION:
        generate_binary_expression(exe, current_block, expr,
                                   OP_GREATERTHAN, ob);
        break;
    case GE_EXPRESSION:
        generate_binary_expression(exe, current_block, expr,
                                   OP_GREATERTHANOREQUAL, ob);
        break;
    case LT_EXPRESSION:
        generate_binary_expression(exe, current_block, expr,
                                   OP_LESSTHAN, ob);
        break;
    case LE_EXPRESSION:
        generate_binary_expression(exe, current_block, expr,
                                   OP_LESSTHANOREQUAL, ob);
    case MINUS_EXPRESSION:
        generate_expression(exe, current_block, expr->u.unary_expression, ob);
        generate_code(ob, OP_NEGATE);
        break;
    case AND_EXPRESSION:
        generate_logical_expression(exe, current_block, expr, ob, OP_AND);
        break;
    case OR_EXPRESSION:
        generate_logical_expression(exe, current_block, expr, ob, OP_OR);
        break;
    case NOT_EXPRESSION:
        generate_expression(exe, current_block, expr->u.unary_expression, ob);
        generate_code(ob, OP_NOT);
        break;
        default:
    printf("expr->kind..%d\n", expr->kind);
    exit(1);
    }
}

static void
generate_expression_statement(SVM_Executable *exe, Block *block,
                              Expression *expr,
                              OpcodeBuf *ob)
{
    generate_expression(exe, block, expr, ob);
    generate_code(ob, OP_DROP);
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
    exe->function_count = compiler->svm_function_count;
    exe->function = compiler->svm_function;

    add_global_variable(compiler, exe);
    add_functions(compiler, exe);
    add_top_level(compiler, exe);

    generate_constant_initializer(compiler, exe);

    return exe;
}
