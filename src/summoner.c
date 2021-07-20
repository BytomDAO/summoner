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

Statement *allocStatement(StatementKind kind) {
    Statement *stmt = malloc(sizeof(Statement));
    stmt->kind = kind;
    return stmt;
}

Statement *allocAssignStatement(char *variable, Expression *operand) {
    Statement *stmt = allocStatement(ASSIGN_STATEMENT);
    AssignStatement *assign_s = malloc(sizeof(AssignStatement));
    stmt->u.assign_s = assign_s;
    return stmt;
}
