#ifndef _SUMMONER_H_
#define _SUMMONER_H_

#include <stdbool.h>
#include <wchar.h>

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
    INTERFACE_TYPE = 1,
    BOOLEAN_TYPE,
    INT_TYPE,
    DOUBLE_TYPE,
    STRING_TYPE,
    STRUCT_TYPE,
} BasicType;

typedef struct TypeSpecifier
{
    BasicType basic_type;
    char *identifier;
} TypeSpecifier;

typedef struct Expression
{
    TypeSpecifier *type;
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

Expression *alloc_expression(ExpressionKind kind);
Expression *alloc_int_expression(int value);
Expression *alloc_double_expression(double value);
Expression *alloc_bool_expression(bool value);
Expression *alloc_identifier_expression(char *identifier);
Expression *alloc_unary_expression(ExpressionKind kind, Expression *unaryExpr);
Expression *alloc_binary_expression(ExpressionKind kind, Expression *left, Expression *right);

typedef enum
{
    ASSIGN_STATEMENT = 1,
    BLOCK_STATEMENT,
    IF_STATEMENT,
    FOR_STATEMENT,
    RETURN_STATEMENT,
    BREAK_STATEMENT,
    CONTINUE_STATEMENT,
    DECLARATION_STATEMENT,
} StatementKind;

typedef struct Declaration
{
    char *name;
    TypeSpecifier *type;
    Expression *initializer;
} Declaration;

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
        struct AssignStatement *assign_s;
        struct Declaration *decl_s;
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
    StatementList *statement_list;
    struct Block *outer_block;
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

Statement *alloc_statement(StatementKind kind);
Statement *alloc_assign_statement(char *variable, Expression *operand);
Statement *alloc_block_statement(Block *block);
Statement *alloc_if_statement(Expression *condition, Block *then_block, Elseif *elseif_list, Block *else_block);
Statement *alloc_declaration_stmt(char *name, TypeSpecifier *type, Expression *initializer);
StatementList *alloc_statement_list(Statement *statement);
StatementList *chain_statement_list(StatementList *list, Statement *statement);
Elseif *alloc_else_if(Expression *condition, Block *block);
Elseif *chain_else_if_list(Elseif *list, Elseif *elseif);
Block *open_block();
Block *close_block(Block *block, StatementList *stmt_list);
Block *alloc_block(StatementList *list);

TypeSpecifier *alloc_type_specifier(BasicType type, char *identifier);

typedef wchar_t SVM_Char;
typedef unsigned char SVM_Byte;

typedef enum {
    SVM_FALSE = 0,
    SVM_TRUE = 1
} SVM_Boolean;

typedef struct {
    TypeSpecifier *type;
    char        *package_name;
    char        *name;
    SVM_Boolean is_defined;
} SVM_Constant;

typedef enum {
    SVM_CONSTANT_INT,
    SVM_CONSTANT_DOUBLE,
    SVM_CONSTANT_STRING
} SVM_ConstantPoolTag;

typedef struct {
    SVM_ConstantPoolTag tag;
    union {
        int     c_int;
        double  c_double;
        SVM_Char *c_string;
    } u;
} SVM_ConstantPool;

typedef struct {
    char                *name;
    TypeSpecifier   *type;
} SVM_Variable;

typedef struct {
    int line_number;
    int start_pc;
    int pc_count;
} SVM_LineNumber;

typedef struct {
    int                 code_size;
    SVM_Byte            *code;
    int                 line_number_size;
    SVM_LineNumber      *line_number;
} SVM_CodeBlock;

// TODO: perfect Compiler
typedef struct Compiler
{
    int                 svm_constant_count;
    SVM_Constant        *svm_constant;
    Block               *current_block;
} Compiler;

Compiler *get_current_compiler();
void set_current_compiler(Compiler *compiler);

typedef struct SVM_Executable {
    int                 constant_pool_count;
    SVM_ConstantPool    *constant_pool;
    int                 global_variable_count;
    SVM_Variable        *global_variable;
    //int               *function_count;
    //SVM_Function      *function;
    int                 type_specifier_count;
    TypeSpecifier       *type_specifier;
    int                 constant_count;
    SVM_Constant        *constant_definition;
    SVM_CodeBlock       top_level;
} SVM_Executable;

#endif
