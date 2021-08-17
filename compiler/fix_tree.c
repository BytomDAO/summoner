#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "summoner.h"
#include "error.h"
#include "../include/DBG.h"

extern BuiltinFun *search_builtin_function(char *name);
static Expression *fix_expression(Block *current_block, Expression *expr);
static void fix_statement_list(Block *current_block, StatementList *list, FuncDefinition *fd);

Declaration *search_declaration_in_current_block(char *identifier, Block *block)
{
    if (!block)
    {
        return NULL;
    }

    Declaration *d_pos;
    for (d_pos = block->declaration_list; d_pos; d_pos = d_pos->next)
    {
        if (!strcmp(identifier, d_pos->name))
        {
            return d_pos;
        }
    }
    return NULL;
}

Declaration *search_declaration(char *identifier, Block *block)
{
    Block *b_pos;
    Declaration *d_pos;
    Compiler *compiler;

    for (b_pos = block; b_pos; b_pos = b_pos->outer_block)
    {
        for (d_pos = b_pos->declaration_list; d_pos; d_pos = d_pos->next)
        {
            if (!strcmp(identifier, d_pos->name))
            {
                return d_pos;
            }
        }
    }

    compiler = get_current_compiler();
    for (d_pos = compiler->declaration_list; d_pos; d_pos = d_pos->next)
    {
        if (!strcmp(identifier, d_pos->name))
        {
            return d_pos;
        }
    }
    return NULL;
}

static Expression *
fix_identifier_expression(Block *current_block, Expression *expr)
{
    Declaration *decl;
    BuiltinFun *builtin_func;

    decl = search_declaration(expr->u.identifier->name, current_block);
    if (decl)
    {
        expr->type = decl->type;
        expr->u.identifier->kind = VARIABLE_IDENTIFIER;
        expr->u.identifier->u.declaration = decl;
        return expr;
    }

    builtin_func = search_builtin_function(expr->u.identifier->name);
    if (builtin_func == NULL)
    {
        compile_error(expr->line_number,
                      IDENTIFIER_NOT_FOUND_ERR,
                      STRING_MESSAGE_ARGUMENT, "name",
                      expr->u.identifier->name,
                      MESSAGE_ARGUMENT_END);
    }
    /* expr->type is not used. function_call_expression's type
     * is gotten from FunctionDefinition in fix_function_call_expression().
     */
    expr->type = alloc_type_specifier(builtin_func->return_type);
    expr->u.identifier->kind = BUILT_IN_FUNCTION;
    expr->u.identifier->u.builtin_func = builtin_func;
    return expr;
}

static Expression *
eval_math_expression_int(Expression *expr, int left, int right)
{
    if (expr->kind == ADD_EXPRESSION)
    {
        expr->u.int_value = left + right;
    }
    else if (expr->kind == SUB_EXPRESSION)
    {
        expr->u.int_value = left - right;
    }
    else if (expr->kind == MUL_EXPRESSION)
    {
        expr->u.int_value = left * right;
    }
    else if (expr->kind == DIV_EXPRESSION)
    {
        if (right == 0)
        {
            compile_error(expr->line_number,
                          DIVISION_BY_ZERO_IN_COMPILE_ERR,
                          MESSAGE_ARGUMENT_END);
        }
        expr->u.int_value = left / right;
    }
    else if (expr->kind == MOD_EXPRESSION)
    {
        expr->u.int_value = left % right;
    }
    else
    {
        DBG_assert(0, ("expr->kind..%d\n", expr->kind));
    }
    expr->kind = INT_EXPRESSION;
    expr->type = alloc_type_specifier(INT_TYPE);
    return expr;
}

static Expression *
eval_math_expression_double(Expression *expr, double left, double right)
{
    if (expr->kind == ADD_EXPRESSION)
    {
        expr->u.double_value = left + right;
    }
    else if (expr->kind == SUB_EXPRESSION)
    {
        expr->u.double_value = left - right;
    }
    else if (expr->kind == MUL_EXPRESSION)
    {
        expr->u.double_value = left * right;
    }
    else if (expr->kind == DIV_EXPRESSION)
    {
        expr->u.double_value = left / right;
    }
    else if (expr->kind == MOD_EXPRESSION)
    {
        expr->u.double_value = fmod(left, right);
    }
    else
    {
        DBG_assert(0, ("expr->kind..%d\n", expr->kind));
    }
    expr->kind = DOUBLE_EXPRESSION;
    expr->type = alloc_type_specifier(DOUBLE_TYPE);
    return expr;
}

static Expression *
chain_string(Expression *expr)
{
    char *left_str = expr->u.binary_expression->left->u.str_value;
    char *right_str = expr->u.binary_expression->right->u.str_value;

    int len = sizeof(left_str) + sizeof(right_str);
    char *new_str = malloc(sizeof(char) * (len + 1));
    strcpy(new_str, left_str);
    strcat(new_str, right_str);
    free(left_str);
    free(right_str);
    expr->kind = STRING_EXPRESSION;
    expr->type = alloc_type_specifier(STRING_TYPE);
    expr->u.str_value = new_str;
    return expr;
}

static Expression *
eval_math_expression(Block *current_block, Expression *expr)
{
    ExpressionKind left_kind = expr->u.binary_expression->left->kind;
    ExpressionKind right_kind = expr->u.binary_expression->right->kind;
    if (left_kind == INT_EXPRESSION && right_kind == INT_EXPRESSION)
    {
        expr = eval_math_expression_int(expr,
                                        expr->u.binary_expression->left
                                            ->u.int_value,
                                        expr->u.binary_expression->right
                                            ->u.int_value);
    }
    else if (left_kind == DOUBLE_EXPRESSION && right_kind == DOUBLE_EXPRESSION)
    {
        expr = eval_math_expression_double(expr,
                                           expr->u.binary_expression->left
                                               ->u.double_value,
                                           expr->u.binary_expression->right
                                               ->u.double_value);
    }
    else if (left_kind == STRING_EXPRESSION && right_kind == STRING_EXPRESSION && expr->kind == ADD_EXPRESSION)
    {
        expr = chain_string(expr);
    }
    return expr;
}

static Expression *
fix_math_binary_expression(Block *current_block, Expression *expr)
{
    expr->u.binary_expression->left = fix_expression(current_block, expr->u.binary_expression->left);
    expr->u.binary_expression->right = fix_expression(current_block, expr->u.binary_expression->right);

    expr = eval_math_expression(current_block, expr);
    if (expr->kind == INT_EXPRESSION || expr->kind == DOUBLE_EXPRESSION || expr->kind == STRING_EXPRESSION)
    {
        return expr;
    }

    BasicType left_type = expr->u.binary_expression->left->type->basic_type;
    BasicType right_type = expr->u.binary_expression->right->type->basic_type;
    if (left_type == INT_TYPE && right_type == INT_TYPE)
    {
        expr->type = alloc_type_specifier(INT_TYPE);
    }
    else if (left_type == DOUBLE_TYPE && right_type == DOUBLE_TYPE)
    {
        expr->type = alloc_type_specifier(DOUBLE_TYPE);
    }
    else if (expr->kind == ADD_EXPRESSION && left_type == STRING_TYPE && right_type == STRING_TYPE)
    {
        expr->type = alloc_type_specifier(STRING_TYPE);
    }
    else
    {
        compile_error(expr->line_number,
                      MATH_TYPE_MISMATCH_ERR,
                      MESSAGE_ARGUMENT_END);
    }
    return expr;
}

static Expression *
eval_compare_expression_boolean(Expression *expr,
                                bool left, bool right)
{
    if (expr->kind == EQ_EXPRESSION)
    {
        expr->u.boolean_value = (left == right);
    }
    else if (expr->kind == NE_EXPRESSION)
    {
        expr->u.boolean_value = (left != right);
    }
    else
    {
        DBG_assert(0, ("expr->kind..%d\n", expr->kind));
    }

    expr->kind = BOOL_EXPRESSION;
    expr->type = alloc_type_specifier(BOOLEAN_TYPE);
    return expr;
}

static Expression *
eval_compare_expression_int(Expression *expr, int left, int right)
{
    if (expr->kind == EQ_EXPRESSION)
    {
        expr->u.boolean_value = (left == right);
    }
    else if (expr->kind == NE_EXPRESSION)
    {
        expr->u.boolean_value = (left != right);
    }
    else if (expr->kind == GT_EXPRESSION)
    {
        expr->u.boolean_value = (left > right);
    }
    else if (expr->kind == GE_EXPRESSION)
    {
        expr->u.boolean_value = (left >= right);
    }
    else if (expr->kind == LT_EXPRESSION)
    {
        expr->u.boolean_value = (left < right);
    }
    else if (expr->kind == LE_EXPRESSION)
    {
        expr->u.boolean_value = (left <= right);
    }
    else
    {
        DBG_assert(0, ("expr->kind..%d\n", expr->kind));
    }

    expr->type = alloc_type_specifier(BOOLEAN_TYPE);
    expr->kind = BOOL_EXPRESSION;
    return expr;
}

static Expression *
eval_compare_expression_double(Expression *expr, double left, double right)
{
    if (expr->kind == EQ_EXPRESSION)
    {
        expr->u.boolean_value = (left == right);
    }
    else if (expr->kind == NE_EXPRESSION)
    {
        expr->u.boolean_value = (left != right);
    }
    else if (expr->kind == GT_EXPRESSION)
    {
        expr->u.boolean_value = (left > right);
    }
    else if (expr->kind == GE_EXPRESSION)
    {
        expr->u.boolean_value = (left >= right);
    }
    else if (expr->kind == LT_EXPRESSION)
    {
        expr->u.boolean_value = (left < right);
    }
    else if (expr->kind == LE_EXPRESSION)
    {
        expr->u.boolean_value = (left <= right);
    }
    else
    {
        DBG_assert(0, ("expr->kind..%d\n", expr->kind));
    }

    expr->kind = BOOL_EXPRESSION;
    expr->type = alloc_type_specifier(BOOLEAN_TYPE);
    return expr;
}

static Expression *
eval_compare_expression_string(Expression *expr,
                               char *left, char *right)
{
    int cmp;

    cmp = strcmp(left, right);

    if (expr->kind == EQ_EXPRESSION)
    {
        expr->u.boolean_value = (cmp == 0);
    }
    else if (expr->kind == NE_EXPRESSION)
    {
        expr->u.boolean_value = (cmp != 0);
    }
    else if (expr->kind == GT_EXPRESSION)
    {
        expr->u.boolean_value = (cmp > 0);
    }
    else if (expr->kind == GE_EXPRESSION)
    {
        expr->u.boolean_value = (cmp >= 0);
    }
    else if (expr->kind == LT_EXPRESSION)
    {
        expr->u.boolean_value = (cmp < 0);
    }
    else if (expr->kind == LE_EXPRESSION)
    {
        expr->u.boolean_value = (cmp <= 0);
    }
    else
    {
        DBG_assert(0, ("expr->kind..%d\n", expr->kind));
    }

    free(left);
    free(right);

    expr->kind = BOOL_EXPRESSION;
    expr->type = alloc_type_specifier(BOOLEAN_TYPE);
    return expr;
}

static Expression *
eval_compare_expression(Expression *expr)
{
    if (expr->u.binary_expression->left->kind == BOOL_EXPRESSION && expr->u.binary_expression->right->kind == BOOL_EXPRESSION)
    {
        expr = eval_compare_expression_boolean(expr,
                                               expr->u.binary_expression->left
                                                   ->u.boolean_value,
                                               expr->u.binary_expression->right
                                                   ->u.boolean_value);
    }
    else if (expr->u.binary_expression->left->kind == INT_EXPRESSION && expr->u.binary_expression->right->kind == INT_EXPRESSION)
    {
        expr = eval_compare_expression_int(expr,
                                           expr->u.binary_expression->left
                                               ->u.int_value,
                                           expr->u.binary_expression->right
                                               ->u.int_value);
    }
    else if (expr->u.binary_expression->left->kind == DOUBLE_EXPRESSION && expr->u.binary_expression->right->kind == DOUBLE_EXPRESSION)
    {
        expr = eval_compare_expression_double(expr,
                                              expr->u.binary_expression->left
                                                  ->u.double_value,
                                              expr->u.binary_expression->right
                                                  ->u.double_value);
    }
    else if (expr->u.binary_expression->left->kind == STRING_EXPRESSION && expr->u.binary_expression->right->kind == STRING_EXPRESSION)
    {
        expr = eval_compare_expression_string(expr,
                                              expr->u.binary_expression->left
                                                  ->u.str_value,
                                              expr->u.binary_expression->right
                                                  ->u.str_value);
    }
    return expr;
}

static Expression *
fix_compare_expression(Block *current_block, Expression *expr)
{
    expr->u.binary_expression->left = fix_expression(current_block, expr->u.binary_expression->left);
    expr->u.binary_expression->right = fix_expression(current_block, expr->u.binary_expression->right);

    expr = eval_compare_expression(expr);
    if (expr->kind == BOOL_EXPRESSION)
    {
        return expr;
    }

    if (expr->u.binary_expression->left->type->basic_type !=
        expr->u.binary_expression->right->type->basic_type)
    {
        compile_error(expr->line_number,
                      COMPARE_TYPE_MISMATCH_ERR,
                      MESSAGE_ARGUMENT_END);
    }

    expr->type = alloc_type_specifier(BOOLEAN_TYPE);
    return expr;
}

static Expression *
fix_logical_and_or_expression(Block *current_block, Expression *expr)
{
    expr->u.binary_expression->left = fix_expression(current_block, expr->u.binary_expression->left);
    expr->u.binary_expression->right = fix_expression(current_block, expr->u.binary_expression->right);

    if (expr->u.binary_expression->left->type->basic_type == BOOLEAN_TYPE &&
        expr->u.binary_expression->right->type->basic_type == BOOLEAN_TYPE)
    {
        expr->type = alloc_type_specifier(BOOLEAN_TYPE);
    }
    else
    {
        compile_error(expr->line_number,
                      LOGICAL_TYPE_MISMATCH_ERR,
                      MESSAGE_ARGUMENT_END);
    }
    return expr;
}

static Expression *
fix_minus_expression(Block *current_block, Expression *expr)
{
    expr->u.unary_expression = fix_expression(current_block, expr->u.unary_expression);
    BasicType expr_type = expr->u.unary_expression->type->basic_type;
    if (expr_type != INT_TYPE && expr_type != DOUBLE_TYPE)
    {
        compile_error(expr->line_number,
                      MINUS_TYPE_MISMATCH_ERR,
                      MESSAGE_ARGUMENT_END);
    }

    expr->type = expr->u.unary_expression->type;
    if (expr->u.unary_expression->kind == INT_EXPRESSION)
    {
        expr->kind = INT_EXPRESSION;
        expr->u.int_value = -expr->u.unary_expression->u.int_value;
    }
    else if (expr->u.unary_expression->kind == DOUBLE_EXPRESSION)
    {
        expr->kind = DOUBLE_EXPRESSION;
        expr->u.double_value = -expr->u.unary_expression->u.double_value;
    }
    return expr;
}

static Expression *
fix_logical_not_expression(Block *current_block, Expression *expr)
{
    expr->u.unary_expression = fix_expression(current_block, expr->u.unary_expression);

    if (expr->u.unary_expression->kind == BOOL_EXPRESSION)
    {
        expr->kind = BOOL_EXPRESSION;
        expr->u.boolean_value = !expr->u.unary_expression->u.boolean_value;
        expr->type = alloc_type_specifier(BOOLEAN_TYPE);
        return expr;
    }

    if (expr->u.unary_expression->type->basic_type != BOOLEAN_TYPE)
    {
        compile_error(expr->line_number,
                      LOGICAL_NOT_TYPE_MISMATCH_ERR,
                      MESSAGE_ARGUMENT_END);
    }
    expr->type = expr->u.unary_expression->type;
    return expr;
}

static Expression *
fix_type_cast_expression(Block *current_block, Expression *expr)
{
    expr->u.unary_expression = fix_expression(current_block, expr->u.unary_expression);
    if (expr->type->basic_type == DOUBLE_TYPE && expr->u.unary_expression->type->basic_type == INT_TYPE)
    {
        if (expr->u.unary_expression->kind == INT_EXPRESSION)
        {
            expr->kind = DOUBLE_EXPRESSION;
            expr->u.double_value = (double)expr->u.unary_expression->u.int_value;
        }
    }
    else if (expr->type->basic_type == INT_TYPE && expr->u.unary_expression->type->basic_type == DOUBLE_TYPE)
    {
        if (expr->u.unary_expression->kind == DOUBLE_EXPRESSION)
        {
            expr->kind = INT_EXPRESSION;
            expr->u.int_value = (int)expr->u.unary_expression->u.double_value;
        }
    }
    else
    {
        compile_error(expr->line_number,
                      TYPE_CAST_MISMATCH_ERR,
                      MESSAGE_ARGUMENT_END);
    }
    return expr;
}

static void
check_argument(Block *current_block, int line_number,
               ParameterList *param_list, ArgumentList *arg)
{
    ParameterList *param;
    TypeSpecifier *temp_type;

    for (param = param_list;
         param && arg;
         param = param->next, arg = arg->next)
    {
        arg->expr = fix_expression(current_block, arg->expr);
        if (param->type->basic_type != arg->expr->type->basic_type)
        {
            compile_error(line_number,
                          FUNCTION_CALL_TYPE_MISMATCH_ERR,
                          MESSAGE_ARGUMENT_END);
        }
    }

    if (param || arg)
    {
        compile_error(line_number,
                      ARGUMENT_COUNT_MISMATCH_ERR,
                      MESSAGE_ARGUMENT_END);
    }
}

static Expression *
fix_function_call_expression(Block *current_block, Expression *expr)
{
    Expression *func_expr = fix_expression(current_block, expr->u.func_call_expression->function);
    expr->u.func_call_expression->function = func_expr;

    BuiltinFun *builtin_fun = func_expr->u.identifier->u.builtin_func;
    if (builtin_fun == NULL)
    {
        compile_error(expr->line_number,
                      FUNCTION_NOT_FOUND_ERR,
                      STRING_MESSAGE_ARGUMENT, "name",
                      func_expr->u.identifier->name,
                      MESSAGE_ARGUMENT_END);
    }

    ParameterList *parameter_list = NULL;
    for (int i = 0; i < builtin_fun->parameter_count; i++)
    {
        parameter_list = chain_parameter(parameter_list, alloc_parameter(alloc_type_specifier(builtin_fun->parameters[i]), ""));
    }

    check_argument(current_block, expr->line_number,
                   parameter_list, expr->u.func_call_expression->argument_list);

    for (ParameterList *pos = parameter_list; pos;)
    {
        ParameterList *old = pos;
        pos = old->next;
        free(old->type);
        free(old);
    }

    expr->type = alloc_type_specifier(builtin_fun->return_type);
    return expr;
}

static Expression *init_expression(BasicType basic_type)
{
    switch (basic_type)
    {
    case AMOUNT_TYPE:
    case INT_TYPE:
        return alloc_int_expression(0);
    case DOUBLE_TYPE:
        return alloc_double_expression(0);
    case STRING_TYPE:
    case ASSET_TYPE:
    case HASH_TYPE:
    case PUBKEY_TYPE:
    case SIG_TYPE:
    case HEX_TYPE:
        return alloc_string_expression("");
    default:
        DBG_assert(0, ("bad case. type..%d\n", basic_type));
    }
    return NULL;
}

static Expression *
fix_expression(Block *current_block, Expression *expr)
{
    if (expr == NULL)
        return NULL;

    switch (expr->kind)
    {
    case BOOL_EXPRESSION:
        expr->type = alloc_type_specifier(BOOLEAN_TYPE);
        break;
    case INT_EXPRESSION:
        expr->type = alloc_type_specifier(INT_TYPE);
        break;
    case DOUBLE_EXPRESSION:
        expr->type = alloc_type_specifier(DOUBLE_TYPE);
        break;
    case STRING_EXPRESSION:
        expr->type = alloc_type_specifier(STRING_TYPE);
        break;
    case IDENTIFIER_EXPRESSION:
        expr = fix_identifier_expression(current_block, expr);
        break;
    case ADD_EXPRESSION: /* FALLTHRU */
    case SUB_EXPRESSION: /* FALLTHRU */
    case MUL_EXPRESSION: /* FALLTHRU */
    case DIV_EXPRESSION: /* FALLTHRU */
    case MOD_EXPRESSION:
        expr = fix_math_binary_expression(current_block, expr);
        break;
    case EQ_EXPRESSION: /* FALLTHRU */
    case NE_EXPRESSION: /* FALLTHRU */
    case GT_EXPRESSION: /* FALLTHRU */
    case GE_EXPRESSION: /* FALLTHRU */
    case LT_EXPRESSION: /* FALLTHRU */
    case LE_EXPRESSION:
        expr = fix_compare_expression(current_block, expr);
        break;
    case AND_EXPRESSION: /* FALLTHRU */
    case OR_EXPRESSION:
        expr = fix_logical_and_or_expression(current_block, expr);
        break;
    case MINUS_EXPRESSION:
        expr = fix_minus_expression(current_block, expr);
        break;
    case NOT_EXPRESSION:
        expr = fix_logical_not_expression(current_block, expr);
        break;
    case TYPE_CAST_EXPRESSION:
        expr = fix_type_cast_expression(current_block, expr);
        break;
    case FUNC_CALL_EXPRESSION:
        expr = fix_function_call_expression(current_block, expr);
        break;
    default:
        DBG_assert(0, ("bad case. kind..%d\n", expr->kind));
    }
    return expr;
}

static void
add_local_variable(FuncDefinition *fd, Declaration *decl)
{
    fd->local_variable = realloc(fd->local_variable,
                                 sizeof(Declaration *) * (fd->local_variable_count + 1));
    fd->local_variable[fd->local_variable_count] = decl;
    decl->variable_index = fd->local_variable_count;
    fd->local_variable_count++;
}

static void
add_declaration(Block *current_block, Declaration *decl,
                FuncDefinition *fd, int line_number)
{
    if (search_declaration_in_current_block(decl->name, current_block))
    {
        compile_error(line_number,
                      VARIABLE_MULTIPLE_DEFINE_ERR,
                      STRING_MESSAGE_ARGUMENT, "name", decl->name,
                      MESSAGE_ARGUMENT_END);
    }

    if (current_block)
    {
        current_block->declaration_list = chain_declaration_list(current_block->declaration_list, decl);
    }
    if (fd)
    {
        decl->is_local = true;
        add_local_variable(fd, decl);
    }
    else
    {
        Compiler *compiler = get_current_compiler();
        decl->is_local = false;
        compiler->declaration_list = chain_declaration_list(compiler->declaration_list, decl);
    }
}

static void
fix_if_statement(Block *current_block, IfStatement *if_s, FuncDefinition *fd)
{
    if_s->condition = fix_expression(current_block, if_s->condition);
    if (if_s->condition->type->basic_type != BOOLEAN_TYPE)
    {
        compile_error(if_s->condition->line_number,
                      IF_CONDITION_NOT_BOOLEAN_ERR,
                      MESSAGE_ARGUMENT_END);
    }

    fix_statement_list(if_s->then_block, if_s->then_block->statement_list, fd);

    for (Elseif *pos = if_s->elseif_list; pos; pos = pos->next)
    {
        pos->condition = fix_expression(current_block, pos->condition);
        if (pos->block)
        {
            fix_statement_list(pos->block, pos->block->statement_list, fd);
        }
    }
    if (if_s->else_block)
    {
        fix_statement_list(if_s->else_block, if_s->else_block->statement_list, fd);
    }
}

static void
fix_return_statement(Block *current_block, Statement *statement, FuncDefinition *fd)
{
    Expression *return_value = fix_expression(current_block, statement->u.expr_s);
    if (fd->return_type->basic_type == VOID_TYPE && return_value != NULL)
    {
        compile_error(statement->line_number,
                      RETURN_IN_VOID_FUNCTION_ERR,
                      MESSAGE_ARGUMENT_END);
    }
    if (fd->return_type->basic_type != VOID_TYPE)
    {
        if (return_value == NULL || return_value->type->basic_type != fd->return_type->basic_type)
            compile_error(statement->line_number,
                          RETURN_TYPE_MISMATCH_ERR,
                          MESSAGE_ARGUMENT_END);
    }

    statement->u.expr_s = return_value;
}

static void
fix_declaration_stmt(Block *current_block, Statement *stmt, FuncDefinition *fd)
{
    Declaration *decl = stmt->u.decl_s;
    add_declaration(current_block, decl, fd, stmt->line_number);
    if (decl->initializer)
    {
        decl->initializer = fix_expression(current_block, decl->initializer);
        if (!decl->type)
        {
            decl->type = decl->initializer->type;
        }
        else if (decl->initializer->kind == INT_EXPRESSION && decl->type->basic_type == DOUBLE_TYPE)
        {
            decl->initializer->kind = DOUBLE_EXPRESSION;
            decl->initializer->type->basic_type = DOUBLE_TYPE;
            decl->initializer->u.double_value = decl->initializer->u.int_value;
        }
        else if (decl->initializer->type->basic_type != decl->type->basic_type)
        {
            compile_error(stmt->line_number,
                          DECLARATION_TYPE_MISMATCH_ERR,
                          MESSAGE_ARGUMENT_END);
        }
    }
    else
    {
        decl->initializer = init_expression(decl->type->basic_type);
    }
}

static void
fix_assign_stmt(Block *current_block, Statement *stmt)
{
    AssignStatement *assign_s = stmt->u.assign_s;
    assign_s->left = fix_expression(current_block, assign_s->left);
    assign_s->operand = fix_expression(current_block, assign_s->operand);
    if (assign_s->operand->kind == INT_EXPRESSION && assign_s->left->type->basic_type == DOUBLE_TYPE)
    {
        assign_s->operand->kind = DOUBLE_EXPRESSION;
        assign_s->operand->type->basic_type = DOUBLE_TYPE;
        assign_s->operand->u.double_value = assign_s->operand->u.int_value;
    }
    else if (assign_s->operand->type->basic_type != assign_s->left->type->basic_type)
    {
        compile_error(stmt->line_number,
                      ASSIGN_TYPE_MISMATCH_ERR,
                      MESSAGE_ARGUMENT_END);
    }
}

static void
fix_statement(Block *current_block, Statement *statement, FuncDefinition *fd)
{
    switch (statement->kind)
    {
    case IF_STATEMENT:
        fix_if_statement(current_block, statement->u.if_s, fd);
        break;
    case RETURN_STATEMENT:
        fix_return_statement(current_block, statement, fd);
        break;
    case DECLARATION_STATEMENT:
        fix_declaration_stmt(current_block, statement, fd);
        break;
    case ASSIGN_STATEMENT:
        fix_assign_stmt(current_block, statement);
        break;
    default:
        DBG_assert(0, ("bad case. kind..%d\n", statement->kind));
    }
}

static void
fix_statement_list(Block *current_block, StatementList *list, FuncDefinition *fd)
{
    StatementList *pos;

    for (pos = list; pos; pos = pos->next)
    {
        fix_statement(current_block, pos->statement, fd);
    }
}

static void
add_parameter_as_declaration(FuncDefinition *fd)
{
    Declaration *decl;
    ParameterList *param;

    for (param = fd->parameters; param; param = param->next)
    {
        decl = alloc_declaration(param->name, param->type, NULL);
        if (fd->block)
        {
            add_declaration(fd->block, decl, fd, param->line_number);
        }
    }
}

static void fix_function(FuncDefinition *fd)
{
    add_parameter_as_declaration(fd);
    if (fd->block)
    {
        fix_statement_list(fd->block, fd->block->statement_list, fd);
    }
}

void fix_tree(Compiler *compiler)
{
    FuncDefinition *func_pos;

    fix_statement_list(NULL, compiler->stmt_list, NULL);

    for (func_pos = compiler->func_definition_list; func_pos; func_pos = func_pos->next)
    {
        fix_function(func_pos);
    }
}