#include <stdio.h>
#include <stdlib.h>
#include "summoner.h"

Expression *allocExpression(ExpressionType type) {
    Expression *expr = malloc(sizeof(Expression));
    expr->type = type;
    return expr;
}

Expression *allocIntExpression(int value) {
    Expression *expr = allocExpression(INT_EXPRESSION);
    expr->u.int_value = value;
    return expr;
}

Expression *allocDoubleExpression(double value) {
    Expression *expr = allocExpression(DOUBLE_EXPRESSION);
    expr->u.double_value = value;
    return expr;
}

Expression *allocBoolExpression(bool value) {
    Expression *expr = allocExpression(BOOL_EXPRESSION);
    expr->u.boolean_value = value;
    return expr;
}

Expression *allocUnaryExpression(ExpressionType type, Expression *unaryExpr) {
    Expression *expr = allocExpression(type);
    expr->u.unary_expression = unaryExpr;
    return expr;
}

Expression *allocBinaryExpression(ExpressionType type, Expression *left, Expression *right) {
    Expression *expr = allocExpression(type);
    BinaryExpression *binary = malloc(sizeof(BinaryExpression));
    binary->left = left;
    binary->right = right;
    expr->u.binary_expression = binary;
    return expr;
}
