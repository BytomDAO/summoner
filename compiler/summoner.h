#ifndef _SUMMONER_H_
#define _SUMMONER_H_

#include <stdbool.h>
#include <wchar.h>

typedef enum
{
    BOOL_EXPRESSION = 1,
    INT_EXPRESSION,
    DOUBLE_EXPRESSION,
    IDENTIFIER_EXPRESSION,
    FUNC_CALL_EXPRESSION,
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
        struct FuncCallExpression *func_call_expression;
    } u;

} Expression;

typedef struct BinaryExpression
{
    Expression *left;
    Expression *right;
} BinaryExpression;

typedef struct ArgumentList
{
    Expression *expr;
    struct ArgumentList *next;
} ArgumentList;

typedef struct FuncCallExpression
{
    char *identifier;
    ArgumentList *argument_list;
} FuncCallExpression;

Expression *alloc_expression(ExpressionKind kind);
Expression *alloc_int_expression(int value);
Expression *alloc_double_expression(double value);
Expression *alloc_bool_expression(bool value);
Expression *alloc_identifier_expression(char *identifier);
Expression *alloc_unary_expression(ExpressionKind kind, Expression *unaryExpr);
Expression *alloc_binary_expression(ExpressionKind kind, Expression *left, Expression *right);
Expression *alloc_func_call_expression(char *identifier, ArgumentList *argument_list);
ArgumentList *chain_argument_list(ArgumentList *list, Expression *expr);

typedef enum
{
    ASSIGN_STATEMENT = 1,
    BLOCK_STATEMENT,
    IF_STATEMENT,
    RETURN_STATEMENT,
    DECLARATION_STATEMENT,
    EXPRESSION_STATEMENT,
} StatementKind;

typedef struct Declaration
{
    char *name;
    TypeSpecifier *type;
    Expression *initializer;
    bool is_local;
    bool is_const;
} Declaration;

typedef struct DeclarationList {
    Declaration *declaration;
    struct DeclarationList *next;
} DeclarationList;

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
        struct Expression *expr_s;
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

Statement *alloc_stmt(StatementKind kind);
Statement *alloc_assign_stmt(char *variable, Expression *operand);
Statement *alloc_block_stmt(Block *block);
Statement *alloc_if_stmt(Expression *condition, Block *then_block, Elseif *elseif_list, Block *else_block);
Statement *alloc_declaration_stmt(char *name, TypeSpecifier *type, Expression *initializer);
Statement *alloc_const_declaration_stmt(char *name, TypeSpecifier *type, Expression *initializer);
Statement *alloc_return_stmt(Expression *expr);
Statement *alloc_expression_stmt(Expression *expr);
StatementList *chain_stmt_list(StatementList *list, Statement *statement);
Elseif *alloc_else_if(Expression *condition, Block *block);
Elseif *chain_else_if_list(Elseif *list, Elseif *elseif);
Block *open_block();
Block *close_block(Block *block, StatementList *stmt_list);
Block *alloc_block(StatementList *list);

TypeSpecifier *alloc_type_specifier(BasicType type, char *identifier);

typedef struct ParameterList
{
    char *name;
    TypeSpecifier *type;
    struct ParameterList *next;
} ParameterList;

ParameterList *alloc_parameter(TypeSpecifier *type, char *identifier);
ParameterList *chain_parameter(ParameterList *list, ParameterList *parameter);

typedef enum
{
    DECLARATION_DEFINITION = 1,
    CONST_DEFINITION,
    FUNC_DEFINITION,
    STRUCT_DEFINITION,
} DefinitionKind;

typedef struct DefinitionList
{
    struct Definition *definition;
    struct DefinitionList *next;
} DefinitionList;

typedef struct Definition
{
    DefinitionKind kind;
    union
    {
        struct FuncDefinition *func_d;
        struct Declaration *declaration;
    } u;
} Definition;

typedef struct FuncDefinition
{
    char *name;
    ParameterList *parameters;
    TypeSpecifier *return_type;
    Block *block;
    struct FuncDefinition *next;
} FuncDefinition;

Definition *alloc_func_definition(char *name, ParameterList *parameters, TypeSpecifier *return_type, Block *block);
FuncDefinition *chain_func_definition_list(FuncDefinition *list, FuncDefinition *next);
Definition *alloc_declaration_definition(Statement *declaration_stmt);
DeclarationList *chain_declaration_definition_list(DeclarationList *list, Declaration *declaration);
DefinitionList *alloc_definition_list(Definition *definition);
DefinitionList *chain_definition_list(DefinitionList *list, Definition *definition);

typedef wchar_t SVM_Char;
typedef unsigned char SVM_Byte;

typedef enum
{
    SVM_FALSE = 0,
    SVM_TRUE = 1
} SVM_Boolean;

typedef struct
{
    TypeSpecifier *type;
    char *package_name;
    char *name;
    SVM_Boolean is_defined;
} SVM_Constant;

typedef enum
{
    SVM_CONSTANT_INT,
    SVM_CONSTANT_DOUBLE,
    SVM_CONSTANT_STRING
} SVM_ConstantPoolTag;

typedef struct
{
    SVM_ConstantPoolTag tag;
    union
    {
        int c_int;
        double c_double;
        SVM_Char *c_string;
    } u;
} SVM_ConstantPool;

typedef struct
{
    char *name;
    TypeSpecifier *type;
} SVM_Variable;

typedef struct
{
    int line_number;
    int start_pc;
    int pc_count;
} SVM_LineNumber;

typedef struct
{
    int code_size;
    SVM_Byte *code;
    int line_number_size;
    SVM_LineNumber *line_number;
} SVM_CodeBlock;

// TODO: perfect Compiler
typedef struct Compiler
{
    int svm_constant_count;
    SVM_Constant *svm_constant;
    int function_count;
    FuncDefinition *func_definition_list;
    Block *current_block;
    DeclarationList     *declaration_list;
} Compiler;

Compiler *create_compiler();
Compiler *get_current_compiler();
void set_current_compiler(Compiler *compiler);
void add_definitions_to_compiler(DefinitionList *definitions);

typedef struct SVM_Executable
{
    int constant_pool_count;
    SVM_ConstantPool *constant_pool;
    int global_variable_count;
    SVM_Variable *global_variable;
    //int               *function_count;
    //SVM_Function      *function;
    int type_specifier_count;
    TypeSpecifier *type_specifier;
    int constant_count;
    SVM_Constant *constant_definition;
    SVM_CodeBlock top_level;
} SVM_Executable;

#endif
