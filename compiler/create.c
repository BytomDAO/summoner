#include <stdio.h>
#include <stdlib.h>
#include "error.h"
#include "summoner.h"

static int current_line_number()
{
    return get_current_compiler()->current_line_number;
}

Expression *alloc_expression(ExpressionKind kind)
{
    Expression *expr = (Expression *)malloc(sizeof(Expression));
    expr->kind = kind;
    expr->line_number = current_line_number();
    return expr;
}

Expression *alloc_int_expression(int value)
{
    Expression *expr = alloc_expression(INT_EXPRESSION);
    expr->u.int_value = value;
    return expr;
}

Expression *alloc_double_expression(double value)
{
    Expression *expr = alloc_expression(DOUBLE_EXPRESSION);
    expr->u.double_value = value;
    return expr;
}

Expression *alloc_identifier_expression(char *identifier)
{
    Expression *expr = alloc_expression(IDENTIFIER_EXPRESSION);
    IdentifierExpression *identifier_expr = (IdentifierExpression *)malloc(sizeof(IdentifierExpression));
    identifier_expr->kind = 0;
    identifier_expr->name = identifier;
    expr->u.identifier = identifier_expr;
    return expr;
}

Expression *alloc_bool_expression(bool value)
{
    Expression *expr = alloc_expression(BOOL_EXPRESSION);
    expr->u.boolean_value = value;
    return expr;
}

Expression *alloc_string_expression(char *value)
{
    Expression *expr = alloc_expression(STRING_EXPRESSION);
    expr->u.str_value = value;
    return expr;
}

Expression *alloc_unary_expression(ExpressionKind kind, Expression *unaryExpr)
{
    Expression *expr = alloc_expression(kind);
    expr->u.unary_expression = unaryExpr;
    return expr;
}

Expression *alloc_type_cast_expression(TypeSpecifier *type, Expression *unaryExpr)
{
    Expression *expr = alloc_expression(TYPE_CAST_EXPRESSION);
    expr->u.unary_expression = unaryExpr;
    expr->type = type;
    return expr;
}

Expression *alloc_binary_expression(ExpressionKind kind, Expression *left, Expression *right)
{
    Expression *expr = alloc_expression(kind);
    BinaryExpression *binary = (BinaryExpression *)malloc(sizeof(BinaryExpression));
    binary->left = left;
    binary->right = right;
    expr->u.binary_expression = binary;
    return expr;
}

Expression *alloc_func_call_expression(Expression *function, ArgumentList *argument_list)
{
    Expression *expr = alloc_expression(FUNC_CALL_EXPRESSION);
    FuncCallExpression *func_call_e = (FuncCallExpression *)malloc(sizeof(FuncCallExpression));
    func_call_e->function = function;
    func_call_e->argument_list = argument_list;
    expr->u.func_call_expression = func_call_e;
    return expr;
}

ArgumentList *chain_argument_list(ArgumentList *list, Expression *expr)
{
    ArgumentList *next = (ArgumentList *)malloc(sizeof(ArgumentList));
    next->expr = expr;
    next->next = NULL;

    if (list == NULL)
    {
        return next;
    }

    ArgumentList *pos;
    for (pos = list; pos->next; pos = pos->next)
        ;

    pos->next = next;
    return list;
}

Statement *alloc_stmt(StatementKind kind)
{
    Statement *stmt = (Statement *)malloc(sizeof(Statement));
    stmt->kind = kind;
    stmt->line_number = current_line_number();
    return stmt;
}

Statement *alloc_assign_stmt(Expression *identifier_expr, Expression *operand)
{
    Statement *stmt = alloc_stmt(ASSIGN_STATEMENT);
    AssignStatement *assign_s = (AssignStatement *)malloc(sizeof(AssignStatement));
    assign_s->left = identifier_expr;
    assign_s->operand = operand;
    stmt->u.assign_s = assign_s;
    return stmt;
}

Statement *alloc_compound_assign_stmt(Expression *identifier_expr, ExpressionKind kind, Expression *operand)
{
    return alloc_assign_stmt(identifier_expr, alloc_binary_expression(kind, identifier_expr, operand));
}

Statement *alloc_if_stmt(Expression *condition, Block *then_block, Elseif *elseif_list, Block *else_block)
{
    Statement *stmt = alloc_stmt(IF_STATEMENT);
    IfStatement *if_s = (IfStatement *)malloc(sizeof(IfStatement));
    if_s->condition = condition;
    if_s->then_block = then_block;
    if_s->elseif_list = elseif_list;
    if_s->else_block = else_block;
    stmt->u.if_s = if_s;
    return stmt;
}

Declaration *alloc_declaration(char *name, TypeSpecifier *type, Expression *initializer)
{
    Declaration *declaration = (Declaration *)malloc(sizeof(Declaration));
    declaration->name = name;
    declaration->type = type;
    declaration->initializer = initializer;
    declaration->next = NULL;
    return declaration;
}

Statement *alloc_declaration_stmt(Declaration *declaration)
{
    Statement *stmt = alloc_stmt(DECLARATION_STATEMENT);
    stmt->u.decl_s = declaration;
    return stmt;
}

Statement *alloc_const_declaration_stmt(char *name, TypeSpecifier *type, Expression *initializer)
{
    return NULL;
}

Statement *alloc_return_stmt(Expression *expr)
{
    Statement *stmt = alloc_stmt(RETURN_STATEMENT);
    stmt->u.expr_s = expr;
    return stmt;
}

Statement *alloc_expression_stmt(Expression *expr)
{
    Statement *stmt = alloc_stmt(EXPRESSION_STATEMENT);
    stmt->u.expr_s = expr;
    return stmt;
}

Statement *alloc_block_stmt(Block *block)
{
    Statement *stmt = alloc_stmt(BLOCK_STATEMENT);
    stmt->u.block_s = block;
    return stmt;
}

StatementList *chain_stmt_list(StatementList *list, Statement *statement)
{
    StatementList *stmt_list = (StatementList *)malloc(sizeof(StatementList));
    stmt_list->statement = statement;
    stmt_list->next = NULL;

    if (list == NULL)
    {
        return stmt_list;
    }

    StatementList *pos;
    for (pos = list; pos->next; pos = pos->next)
        ;

    pos->next = stmt_list;
    return list;
}

Elseif *alloc_else_if(Expression *condition, Block *block)
{
    Elseif *elseif = (Elseif *)malloc(sizeof(Elseif));
    elseif->condition = condition;
    elseif->block = block;
    return elseif;
}

Elseif *chain_else_if_list(Elseif *list, Elseif *elseif)
{
    if (list == NULL)
    {
        return elseif;
    }

    Elseif *pos;
    for (pos = list; pos->next; pos = pos->next)
        ;

    pos->next = elseif;
    return list;
}

Block *open_block()
{
    Block *new_block;
    Compiler *compiler = get_current_compiler();
    new_block = alloc_block(NULL);
    printf("open:%p\n", new_block);
    new_block->outer_block = compiler->current_block;
//    printf("open out:%p\n", compiler->current_block);
    compiler->current_block = new_block;
    return new_block;
}

Block *close_block(Block *block, StatementList *stmt_list)
{
    printf("close:%p\n", block);
    Compiler *compiler = get_current_compiler();
    block->statement_list = stmt_list;
    compiler->current_block = block->outer_block;
    return block;
}

Block *alloc_block(StatementList *list)
{
    Block *block = (Block *)malloc(sizeof(Block));
    block->statement_list = list;
    block->declaration_list = NULL;
    return block;
}

TypeSpecifier *alloc_type_specifier(BasicType type)
{
    TypeSpecifier *type_s = (TypeSpecifier *)malloc(sizeof(TypeSpecifier));
    type_s->basic_type = type;
    return type_s;
}

ParameterList *alloc_parameter(TypeSpecifier *type, char *identifier)
{
    ParameterList *p = (ParameterList *)malloc(sizeof(ParameterList));
    p->name = identifier;
    p->type = type;
    p->next = NULL;
    return p;
}

ParameterList *chain_parameter(ParameterList *list, ParameterList *parameter)
{
    if (list == NULL)
    {
        return parameter;
    }
    ParameterList *pos;
    for (pos = list; pos->next; pos = pos->next)
        ;
    pos->next = parameter;
    return list;
}

FuncDefinition *alloc_func_definition(char *name, ParameterList *parameters, TypeSpecifier *return_type, Block *block)
{
    if (search_function(name) || search_declaration(name, NULL))
    {
        compile_error(get_current_compiler()->current_line_number,
                          FUNCTION_MULTIPLE_DEFINE_ERR,
                          STRING_MESSAGE_ARGUMENT, "name", name,
                          MESSAGE_ARGUMENT_END);
    }

    FuncDefinition *func_d = (FuncDefinition *)malloc(sizeof(FuncDefinition));
    func_d->name = name;
    func_d->parameters = parameters;
    func_d->return_type = return_type;
    if (!return_type)
    {
        func_d->return_type = alloc_type_specifier(VOID_TYPE);
    }
    func_d->code_block = block;
    func_d->local_variable_count = 0;
    func_d->local_variable = NULL;
    func_d->next = NULL;
    return func_d;
}

FuncDefinition *chain_func_definition_list(FuncDefinition *list, FuncDefinition *next)
{
    if (list == NULL)
    {
        return next;
    }

    FuncDefinition *pos;
    for (pos = list; pos->next; pos = pos->next)
        ;

    pos->next = next;
    return list;
}

Definition *alloc_definition()
{
    return (Definition *)malloc(sizeof(Definition));
}

DefinitionList *alloc_definition_list()
{
    return (DefinitionList *)malloc(sizeof(DefinitionList));
}

Declaration *chain_declaration_list(Declaration *list, Declaration *next)
{
    if (list == NULL)
    {
        return next;
    }

    Declaration *pos;
    for (pos = list; pos->next; pos = pos->next)
        ;

    pos->next = next;
    return list;
}
