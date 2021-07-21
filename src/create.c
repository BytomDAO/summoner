#include <stdio.h>
#include <stdlib.h>
#include "summoner.h"

Expression *alloc_expression(ExpressionKind kind)
{
    Expression *expr = malloc(sizeof(Expression));
    expr->kind = kind;
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
    expr->u.identifier = identifier;
    return expr;
}

Expression *alloc_bool_expression(bool value)
{
    Expression *expr = alloc_expression(BOOL_EXPRESSION);
    expr->u.boolean_value = value;
    return expr;
}

Expression *alloc_unary_expression(ExpressionKind kind, Expression *unaryExpr)
{
    Expression *expr = alloc_expression(kind);
    expr->u.unary_expression = unaryExpr;
    return expr;
}

Expression *alloc_binary_expression(ExpressionKind kind, Expression *left, Expression *right)
{
    Expression *expr = alloc_expression(kind);
    BinaryExpression *binary = malloc(sizeof(BinaryExpression));
    binary->left = left;
    binary->right = right;
    expr->u.binary_expression = binary;
    return expr;
}

Statement *alloc_statement(StatementKind kind)
{
    Statement *stmt = malloc(sizeof(Statement));
    stmt->kind = kind;
    return stmt;
}

Statement *alloc_assign_statement(char *variable, Expression *operand)
{
    Statement *stmt = alloc_statement(ASSIGN_STATEMENT);
    AssignStatement *assign_s = malloc(sizeof(AssignStatement));
    stmt->u.assign_s = assign_s;
    return stmt;
}

Statement *alloc_if_statement(Expression *condition, Block *then_block, Elseif *elseif_list, Block *else_block)
{
    Statement *stmt = alloc_statement(IF_STATEMENT);
    IfStatement *if_s = malloc(sizeof(IfStatement));
    if_s->condition = condition;
    if_s->then_block = then_block;
    if_s->elseif_list = elseif_list;
    if_s->else_block = else_block;
    stmt->u.if_s = if_s;
    return stmt;
}

Statement *alloc_block_statement(Block *block)
{
    Statement *stmt = alloc_statement(BLOCK_STATEMENT);
    stmt->u.block_s = block;
    return stmt;
}

StatementList *alloc_statement_list(Statement *statement)
{
    StatementList *stmt_list = malloc(sizeof(StatementList));
    stmt_list->statement = statement;
    return stmt_list;
}

StatementList *chain_statement_list(StatementList *list, Statement *statement)
{
    if (list == NULL)
    {
        return alloc_statement_list(statement);
    }

    StatementList *pos;
    for (pos = list; pos->next; pos = pos->next)
        ;

    pos->next = alloc_statement_list(statement);
    return list;
}

Elseif *alloc_else_if(Expression *condition, Block *block)
{
    Elseif *elseif = malloc(sizeof(Elseif));
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

Block *alloc_block(StatementList *list)
{
    Block *block = malloc(sizeof(Block));
    block->statemen_list = list;
    return block;
}
