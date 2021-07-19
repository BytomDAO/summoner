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
    IDENTIFIER_EXPRESSION,
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
    AND_EXPRESSION, /* && */
    OR_EXPRESSION,  /* || */
    NOT_EXPRESSION, /* ! */
} ExpressionKind;

typedef enum
{
    BOOL_TYPE,
    INT_TYPE,
    DOUBLE_TYPE,
    STRING_TYPE,
} BasicType;

typedef struct TypeSpecifier
{
    BasicType basic_type;
} TypeSpecifier;

typedef struct BinaryExpression BinaryExpression;

typedef struct Expression
{
    ExpressionKind kind;
    union
    {
        bool boolean_value;
        int int_value;
        double double_value;
        char *identifier;
        BinaryExpression *binary_expression;
        struct Expression *unary_expression;
    } u;

} Expression;

typedef struct BinaryExpression
{
    Expression *left;
    Expression *right;
} BinaryExpression;

Expression *allocExpression(ExpressionKind kind);
Expression *allocIntExpression(int value);
Expression *allocDoubleExpression(double value);
Expression *allocBoolExpression(bool value);
Expression *allocIdentifierExpression(char *identifier);
Expression *allocUnaryExpression(ExpressionKind kind, Expression *unaryExpr);
Expression *allocBinaryExpression(ExpressionKind kind, Expression *left, Expression *right);

typedef enum
{
    ASSIGN_STATEMENT = 1,
    IF_STATEMENT,
    FOR_STATEMENT,
    RETURN_STATEMENT,
    BREAK_STATEMENT,
    CONTINUE_STATEMENT,
} StatementKind;

typedef struct AssignStatement
{
    char *variable;
    Expression *operand;
} AssignStatement;

typedef struct Statement
{
    StatementKind kind;
    union
    {
        AssignStatement *assign_s;
    } u;
} Statement;

typedef struct StatementList
{
    Statement *statement;
    struct StatementList *next;
} StatementList;

Statement *allocStatement(StatementKind kind);
Statement *allocAssignStatement(char *variable, Expression *operand);

#endif
