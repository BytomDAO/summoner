#ifndef _SUMMONER_H_
#define _SUMMONER_H_

#include <stdbool.h>
#include <wchar.h>

typedef enum
{
    BOOL_EXPRESSION = 1,
    INT_EXPRESSION,
    DOUBLE_EXPRESSION,
    STRING_EXPRESSION,
    IDENTIFIER_EXPRESSION,
    FUNC_CALL_EXPRESSION,
    ADD_EXPRESSION,
    SUB_EXPRESSION,
    MUL_EXPRESSION,
    DIV_EXPRESSION,
    MINUS_EXPRESSION,
    TYPE_CAST_EXPRESSION,
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
    ASSET_TYPE,
    HASH_TYPE,
    AMOUNT_TYPE,
    PUBKEY_TYPE,
    SIG_TYPE,
    HEX_TYPE,
    VOID_TYPE,
    STRUCT_TYPE,
} BasicType;

typedef struct TypeSpecifier
{
    BasicType basic_type;
} TypeSpecifier;

typedef struct Expression
{
    TypeSpecifier *type;
    ExpressionKind kind;
    int line_number;
    union
    {
        bool boolean_value;
        int int_value;
        double double_value;
        char *str_value;
        struct BinaryExpression *binary_expression;
        struct Expression *unary_expression;
        struct FuncCallExpression *func_call_expression;
        struct IdentifierExpression *identifier;
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
    Expression *function;
    ArgumentList *argument_list;
} FuncCallExpression;

typedef enum
{
    VARIABLE_IDENTIFIER,
    BUILT_IN_FUNCTION,
} IdentifierKind;

typedef struct BuiltinFun
{
    const char *name;
    BasicType *parameters;
    int parameter_count;
    BasicType return_type;
    const char *op_codes;
} BuiltinFun;

typedef struct IdentifierExpression
{
    char *name;
    IdentifierKind kind;
    union
    {
        struct Declaration *declaration;
        BuiltinFun *builtin_func;
    } u;
} IdentifierExpression;

Expression *alloc_expression(ExpressionKind kind);
Expression *alloc_int_expression(int value);
Expression *alloc_double_expression(double value);
Expression *alloc_bool_expression(bool value);
Expression *alloc_string_expression(char *value);
Expression *alloc_identifier_expression(char *identifier);
Expression *alloc_type_cast_expression(TypeSpecifier *type, Expression *expr);
Expression *alloc_unary_expression(ExpressionKind kind, Expression *unaryExpr);
Expression *alloc_binary_expression(ExpressionKind kind, Expression *left, Expression *right);
Expression *alloc_func_call_expression(Expression *identifier_expr, ArgumentList *argument_list);
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
    int variable_index;
    struct Declaration *next;
} Declaration;

typedef struct AssignStatement
{
    Expression *left;
    Expression *operand;
} AssignStatement;

typedef struct Statement
{
    StatementKind kind;
    int line_number;
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
    Declaration *declaration_list;
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
Statement *alloc_assign_stmt(Expression *identifier_expr, Expression *operand);
Statement *alloc_compound_assign_stmt(Expression *identifier_expr, ExpressionKind kind, Expression *operand);
Statement *alloc_block_stmt(Block *block);
Statement *alloc_if_stmt(Expression *condition, Block *then_block, Elseif *elseif_list, Block *else_block);
Declaration *alloc_declaration(char *name, TypeSpecifier *type, Expression *initializer);
Statement *alloc_declaration_stmt(Declaration *declaration);
Statement *alloc_const_declaration_stmt(char *name, TypeSpecifier *type, Expression *initializer);
Statement *alloc_return_stmt(Expression *expr);
Statement *alloc_expression_stmt(Expression *expr);
StatementList *chain_stmt_list(StatementList *list, Statement *statement);
Elseif *alloc_else_if(Expression *condition, Block *block);
Elseif *chain_else_if_list(Elseif *list, Elseif *elseif);
Block *open_block();
Block *close_block(Block *block, StatementList *stmt_list);
Block *alloc_block(StatementList *list);

TypeSpecifier *alloc_type_specifier(BasicType type);

typedef struct ParameterList
{
    char *name;
    TypeSpecifier *type;
    int line_number;
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

typedef struct FuncDefinition
{
    char *name;
    ParameterList *parameters;
    TypeSpecifier *return_type;
    Block *block;
    int local_variable_count;
    Declaration **local_variable;
    struct FuncDefinition *next;
} FuncDefinition;

FuncDefinition *alloc_func_definition(char *name, ParameterList *parameters, TypeSpecifier *return_type, Block *block);
FuncDefinition *chain_func_definition_list(FuncDefinition *list, FuncDefinition *next);
Declaration *chain_declaration_list(Declaration *list, Declaration *declaration);

typedef struct Definition {} Definition;
typedef struct DefinitionList {} DefinitionList;

Definition *alloc_definition();
DefinitionList *alloc_definition_list();

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

typedef struct
{
    TypeSpecifier *type;
    char *name;
    int parameter_count;
    SVM_Variable *parameter;
    int local_variable_count;
    SVM_Variable *local_variable;
    SVM_CodeBlock code_block;
} SVM_Function;

// TODO: perfect Compiler
typedef struct Compiler
{
    int svm_constant_count;
    SVM_Constant *svm_constant;
    int function_count;
    FuncDefinition *func_definition_list;
    StatementList *stmt_list;
    Block *current_block;
    Declaration *declaration_list;
    int current_line_number;
} Compiler;

Compiler *create_compiler();
Compiler *get_current_compiler();
void increment_line_number();
void set_current_compiler(Compiler *compiler);
void add_stmt_to_compiler(Statement *stmt);
void add_func_definition_to_compiler(FuncDefinition *func_definition);

typedef struct SVM_Executable
{
    int constant_pool_count;
    SVM_ConstantPool *constant_pool;
    int global_variable_count;
    SVM_Variable *global_variable;
    int function_count;
    SVM_Function *function;
    int type_specifier_count;
    TypeSpecifier *type_specifier;
    int constant_count;
    SVM_Constant *constant_definition;
    SVM_CodeBlock top_level;
} SVM_Executable;

/** string.c */
void open_string_literal(void);
void add_string_literal(int letter);
char *close_string_literal(void);

/** fix_tree.c */
void fix_tree(Compiler *compiler);

/** codegen.c */
SVM_Executable *smc_code_gen(Compiler *compiler);

/* disassemble.c */
int svm_dump_instruction(FILE *fp, SVM_Byte *code, int index);
void svm_disassemble(SVM_Executable *exe);

#endif
