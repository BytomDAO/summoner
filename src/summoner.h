#ifndef _SUMMONER_H_
#define _SUMMONER_H_

#include <stdbool.h>

typedef enum
{
    EXPR_BOOL_VALUE = 1,
    EXPR_INT_VALUE,
    EXPR_DOUBLE_VALUE,
} ExprValueType;

typedef struct ExprValue
{
    ExprValueType type;
    union
    {
        bool boolean_value;
        int int_value;
        double double_value;
    } u;

} ExprValue;

typedef enum
{
    BOOL_EXPRESSION = 1,
    INT_EXPRESSION,
    DOUBLE_EXPRESSION,
    ADD_EXPRESSION,
    SUB_EXPRESSION,
    MUL_EXPRESSION,
    DIV_EXPRESSION,
    MINUS_EXPRESSION,
    MOD_EXPRESSION, /* % */
    EQ_EXPRESSION,  /* == */
    NE_EXPRESSION,  /* != */
    GT_EXPRESSION,  /* > */
    GE_EXPRESSION,  /* >= */
    LT_EXPRESSION,  /* < */
    LE_EXPRESSION,  /* <= */

} ExpressionType;

typedef struct BinaryExpression BinaryExpression;

typedef struct Expression
{
    ExpressionType type;
    union
    {
        bool boolean_value;
        int int_value;
        double double_value;
        BinaryExpression *binary_expression;
        struct Expression *unary_expression;
    } u;

} Expression;

typedef struct BinaryExpression
{
    Expression *left;
    Expression *right;
} BinaryExpression;

Expression *allocExpression(ExpressionType type);
Expression *allocIntExpression(int value);
Expression *allocDoubleExpression(double value);
Expression *allocBoolExpression(bool value);
Expression *allocUnaryExpression(ExpressionType type, Expression *unaryExpr);
Expression *allocBinaryExpression(ExpressionType type, Expression *left, Expression *right);

#endif
