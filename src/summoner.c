#include <stdio.h>
#include <stdlib.h>
#include "summoner.h"

Expression *allocExpression(ExpressionKind kind)
{
    Expression *expr = malloc(sizeof(Expression));
    expr->kind = kind;
    return expr;
}

Expression *allocIntExpression(int value)
{
    Expression *expr = allocExpression(INT_EXPRESSION);
    expr->u.int_value = value;
    return expr;
}

Expression *allocDoubleExpression(double value)
{
    Expression *expr = allocExpression(DOUBLE_EXPRESSION);
    expr->u.double_value = value;
    return expr;
}

Expression *allocIdentifierExpression(char *identifier)
{
    Expression *expr = allocExpression(IDENTIFIER_EXPRESSION);
    expr->u.identifier = identifier;
    return expr;
}

Expression *allocBoolExpression(bool value)
{
    Expression *expr = allocExpression(BOOL_EXPRESSION);
    expr->u.boolean_value = value;
    return expr;
}

Expression *allocUnaryExpression(ExpressionKind kind, Expression *unaryExpr)
{
    Expression *expr = allocExpression(kind);
    expr->u.unary_expression = unaryExpr;
    return expr;
}

Expression *allocBinaryExpression(ExpressionKind kind, Expression *left, Expression *right)
{
    Expression *expr = allocExpression(kind);
    BinaryExpression *binary = malloc(sizeof(BinaryExpression));
    binary->left = left;
    binary->right = right;
    expr->u.binary_expression = binary;
    return expr;
}

Statement *allocStatement(StatementKind kind)
{
    Statement *stmt = malloc(sizeof(Statement));
    stmt->kind = kind;
    return stmt;
}

Statement *allocAssignStatement(char *variable, Expression *operand)
{
    Statement *stmt = allocStatement(ASSIGN_STATEMENT);
    AssignStatement *assign_s = malloc(sizeof(AssignStatement));
    stmt->u.assign_s = assign_s;
    return stmt;
}

Statement *allocIfStatement(Expression *condition, Block *then_block, Elseif *elseif_list, Block *else_block)
{
    Statement *stmt = allocStatement(IF_STATEMENT);
    IfStatement *if_s = malloc(sizeof(IfStatement));
    if_s->condition = condition;
    if_s->then_block = then_block;
    if_s->elseif_list = elseif_list;
    if_s->else_block = else_block;
    stmt->u.if_s = if_s;
    return stmt;
}

Statement *allocBlockStatement(Block *block)
{
    Statement *stmt = allocStatement(BLOCK_STATEMENT);
    stmt->u.block_s = block;
    return stmt;
}

StatementList *allocStatementList(Statement *statement)
{
    StatementList *stmt_list = malloc(sizeof(StatementList));
    stmt_list->statement = statement;
    return stmt_list;
}

StatementList *chainStatementList(StatementList *list, Statement *statement)
{
    if (list == NULL)
    {
        return allocStatementList(statement);
    }

    StatementList *pos;
    for (pos = list; pos->next; pos = pos->next)
        ;

    pos->next = allocStatementList(statement);
    return list;
}

Elseif *allocElseif(Expression *condition, Block *block)
{
    Elseif *elseif = malloc(sizeof(Elseif));
    elseif->condition = condition;
    elseif->block = block;
    return elseif;
}

Elseif *chainElseifList(Elseif *list, Elseif *elseif)
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

Block *allocBlock(StatementList *list)
{
    Block *block = malloc(sizeof(Block));
    block->statemen_list = list;
    return block;
}
