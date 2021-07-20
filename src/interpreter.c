
#include <stdio.h>
#include <stdlib.h>
#include "interpreter.h"
#include "summoner.h"

ExprValue evalIntBinaryExpression(ExpressionKind type, int left, int right)
{
    ExprValue v;
    v.type = EXPR_INT_VALUE;
    switch (type)
    {
    case ADD_EXPRESSION:
        v.u.int_value = left + right;
        break;
    case SUB_EXPRESSION:
        v.u.int_value = left - right;
        break;
    case MUL_EXPRESSION:
        v.u.int_value = left * right;
        break;
    case DIV_EXPRESSION:
        v.u.int_value = left / right;
        break;
    case LT_EXPRESSION:
        v.type = EXPR_BOOL_VALUE;
        v.u.boolean_value = left < right;
        break;
    case LE_EXPRESSION:
        v.type = EXPR_BOOL_VALUE;
        v.u.boolean_value = left <= right;
        break;
    case GT_EXPRESSION:
        v.type = EXPR_BOOL_VALUE;
        v.u.boolean_value = left > right;
        break;
    case GE_EXPRESSION:
        v.type = EXPR_BOOL_VALUE;
        v.u.boolean_value = left >= right;
        break;
    case EQ_EXPRESSION:
        v.type = EXPR_BOOL_VALUE;
        v.u.boolean_value = left == right;
        break;
    case NE_EXPRESSION:
        v.type = EXPR_BOOL_VALUE;
        v.u.boolean_value = left != right;
        break;
    default:
        printf("invalid expression type\n");
        exit(1);
    }
    return v;
}

ExprValue evalDoubleBinaryExpression(ExpressionKind kind, double left, double right)
{
    ExprValue v;
    v.type = EXPR_DOUBLE_VALUE;
    switch (kind)
    {
    case ADD_EXPRESSION:
        v.u.double_value = left + right;
        break;
    case SUB_EXPRESSION:
        v.u.double_value = left - right;
        break;
    case MUL_EXPRESSION:
        v.u.double_value = left * right;
        break;
    case DIV_EXPRESSION:
        v.u.double_value = left / right;
        break;
    case LT_EXPRESSION:
        v.type = EXPR_BOOL_VALUE;
        v.u.boolean_value = left < right;
        break;
    case LE_EXPRESSION:
        v.type = EXPR_BOOL_VALUE;
        v.u.boolean_value = left <= right;
        break;
    case GT_EXPRESSION:
        v.type = EXPR_BOOL_VALUE;
        v.u.boolean_value = left > right;
        break;
    case GE_EXPRESSION:
        v.type = EXPR_BOOL_VALUE;
        v.u.boolean_value = left >= right;
        break;
    case EQ_EXPRESSION:
        v.type = EXPR_BOOL_VALUE;
        v.u.boolean_value = left == right;
        break;
    case NE_EXPRESSION:
        v.type = EXPR_BOOL_VALUE;
        v.u.boolean_value = left != right;
        break;
    default:
        printf("invalid expression type\n");
        exit(1);
    }
    return v;
}

ExprValue evalBoolBinaryExpression(ExpressionKind type, bool left, bool right)
{
    ExprValue v;
    v.type = EXPR_BOOL_VALUE;
    switch (type)
    {
    case AND_EXPRESSION:
        v.u.boolean_value = left && right;
        break;
    case OR_EXPRESSION:
        v.u.boolean_value = left || right;
        break;
    default:
        printf("invalid expression type when eval bool value:%d\n", type);
        exit(1);
    }
    return v;
}

ExprValue evalBinaryExpression(ExpressionKind type, BinaryExpression *binaryExpression)
{
    ExprValue leftVal = evalExpression(binaryExpression->left);
    ExprValue rightVal = evalExpression(binaryExpression->right);
    if (leftVal.type == EXPR_INT_VALUE && rightVal.type == EXPR_INT_VALUE)
    {
        return evalIntBinaryExpression(type, leftVal.u.int_value, rightVal.u.int_value);
    }

    if (leftVal.type == EXPR_INT_VALUE && rightVal.type == EXPR_DOUBLE_VALUE)
    {
        return evalDoubleBinaryExpression(type, leftVal.u.int_value, rightVal.u.double_value);
    }

    if (leftVal.type == EXPR_DOUBLE_VALUE && rightVal.type == EXPR_INT_VALUE)
    {
        return evalDoubleBinaryExpression(type, leftVal.u.double_value, rightVal.u.int_value);
    }

    if (leftVal.type == EXPR_DOUBLE_VALUE && rightVal.type == EXPR_DOUBLE_VALUE)
    {
        return evalDoubleBinaryExpression(type, leftVal.u.double_value, rightVal.u.double_value);
    }

    if (leftVal.type == EXPR_BOOL_VALUE && rightVal.type == EXPR_BOOL_VALUE)
    {
        return evalBoolBinaryExpression(type, leftVal.u.boolean_value, rightVal.u.boolean_value);
    }

    printf("invalid expression type, left:%d, right:%d", leftVal.type, rightVal.type);
    exit(1);
}

ExprValue evalExpression(Expression *expr)
{
    ExprValue v;
    switch (expr->kind)
    {
    case BOOL_EXPRESSION:
        v.type = EXPR_BOOL_VALUE;
        v.u.boolean_value = expr->u.boolean_value;
        return v;
    case INT_EXPRESSION:
        v.type = EXPR_INT_VALUE;
        v.u.int_value = expr->u.int_value;
        return v;
    case DOUBLE_EXPRESSION:
        v.type = EXPR_DOUBLE_VALUE;
        v.u.double_value = expr->u.double_value;
        return v;
    case MINUS_EXPRESSION:
        v = evalExpression(expr->u.unary_expression);
        if (v.type == EXPR_INT_VALUE)
        {
            v.u.int_value = -v.u.int_value;
        }
        else
        {
            v.u.double_value = -v.u.double_value;
        }
        return v;
    case NOT_EXPRESSION:
        v = evalExpression(expr->u.unary_expression);
        v.u.boolean_value = !v.u.boolean_value;
        return v;
    case ADD_EXPRESSION:
    case SUB_EXPRESSION:
    case MUL_EXPRESSION:
    case DIV_EXPRESSION:
    case LT_EXPRESSION:
    case LE_EXPRESSION:
    case GT_EXPRESSION:
    case GE_EXPRESSION:
    case EQ_EXPRESSION:
    case NE_EXPRESSION:
    case AND_EXPRESSION:
    case OR_EXPRESSION:
        return evalBinaryExpression(expr->kind, expr->u.binary_expression);
    default:
        printf("invalid expression type when eval expression:%d\n", expr->kind);
        exit(1);
    }
}

void printExprValue(ExprValue val)
{
    switch (val.type)
    {
    case EXPR_INT_VALUE:
        printf(">>>%d\n", val.u.int_value);
        break;
    case EXPR_DOUBLE_VALUE:
        printf(">>>%lf\n", val.u.double_value);
        break;
    case EXPR_BOOL_VALUE:
        printf(">>>%s\n", val.u.boolean_value ? "true" : "false");
        break;
    default:
        printf("invalid expression type when print expr value:%d", val.type);
        exit(1);
    }
}
