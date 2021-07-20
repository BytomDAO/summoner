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

typedef struct Expression
{
    ExpressionKind kind;
    union
    {
        bool boolean_value;
        int int_value;
        double double_value;
        char *identifier;
        struct BinaryExpression *binary_expression;
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
    BLOCK_STATEMENT,
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
        struct Block *block_s;
        struct IfStatement *if_s;
    } u;
} Statement;

typedef struct StatementList
{
    Statement *statement;
    struct StatementList *next;
} StatementList;

typedef struct Block
{
    StatementList *statemen_list;
} Block;

typedef struct Elseif
{
    Expression *condition;
    Block *block;
    struct Elseif *next;
} Elseif;

typedef struct IfStatement
{
    Expression *condition;
    Block *then_block;
    Elseif *elseif_list;
    Block *else_block;
} IfStatement;

Statement *allocStatement(StatementKind kind);
Statement *allocAssignStatement(char *variable, Expression *operand);
Statement *allocBlockStatement(Block *block);
Statement *allocIfStatement(Expression *condition, Block *then_block, Elseif *elseif_list, Block *else_block);
StatementList *allocStatementList(Statement *statement);
StatementList *chainStatementList(StatementList *list, Statement *statement);
Elseif *allocElseif(Expression *condition, Block *block);
Elseif *chainElseifList(Elseif *list, Elseif *elseif);
Block *allocBlock(StatementList *list);

#endif
