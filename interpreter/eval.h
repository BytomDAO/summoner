#ifndef _EVAL_H_
#define _EVAL_H_

#include <map>
#include <string>
#include <vector>
#include "../compiler/summoner.h"

using namespace std;

typedef enum
{
    EXPR_BOOL_VALUE = 1,
    EXPR_INT_VALUE,
    EXPR_DOUBLE_VALUE,
    EXPR_STRING_VALUE,
} ExprValueType;

typedef struct ExprValue
{
    ExprValueType type;
    union
    {
        bool boolean_value;
        int int_value;
        double double_value;
        char *str_value;
    } u;

} ExprValue;

typedef enum
{
    NORMAL_STATEMENT_RESULT = 1,
    RETURN_STATEMENT_RESULT,
} StmtResultType;

typedef struct StmtResult
{
    StmtResultType type;
    union
    {
        ExprValue return_value;
    } u;
} StmtResult;

class Identifier
{
public:
    Identifier(string name, ExprValue value) : name(name), value(value) {}
    Identifier(string name, ExprValue value, bool is_const) : name(name), value(value), is_const(is_const) {}
    void set_value(ExprValue value) { this->value = value; }
    string get_name() { return this->name; }
    ExprValue get_value() { return this->value; }
    bool get_is_const() { return this->is_const; }

private:
    string name;
    ExprValue value;
    bool is_const;
};

class Scope
{
public:
    Scope(Scope *parent_scope) : parent_scope(parent_scope) {}

    Identifier *find_identifier(char *name)
    {
        if (this->identifier_dict.find(name) != this->identifier_dict.end())
        {
            return this->identifier_dict[name];
        }
        if (this->parent_scope == nullptr)
        {
            return nullptr;
        }
        return this->parent_scope->find_identifier(name);
    };
    void put_identifier(Identifier *identifier) { identifier_dict[identifier->get_name()] = identifier; }

private:
    map<string, Identifier *> identifier_dict;
    Scope *parent_scope;
};

class Interpreter
{
public:
    Interpreter(FuncDefinition *func_list, Declaration *declaration_list) : func_definition_list(func_list), declaration_list(declaration_list), current_scope(new Scope(nullptr)){};
    void exec();

private:
    Scope *current_scope;
    FuncDefinition *func_definition_list;
    Declaration *declaration_list;

    ExprValue *eval_func(FuncDefinition *fd, ArgumentList *arg_list);
    FuncDefinition *find_func_definition(const char *name);
    StmtResult eval_block(Block *block, vector<Identifier *> identifiers);
    StmtResult eval_stmt(Statement *stmt);
    ExprValue eval_expression(Expression *expr);
    ExprValue eval_binary_expression(ExpressionKind kind, BinaryExpression *expr);
};

#endif
