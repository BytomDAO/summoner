#include <stdio.h>
#include <stdlib.h>
#include "summoner.h"

Expression *alloc_expression(ExpressionKind kind)
{
    Expression *expr = (Expression *)malloc(sizeof(Expression));
    expr->kind = kind;
    return expr;
}

Expression *alloc_int_expression(int value)
{
    Expression *expr = alloc_expression(INT_EXPRESSION);
    expr->u.int_value = value;
    return expr;
}

Expression *alloc_double_expression(double value)
{
    Expression *expr = alloc_expression(DOUBLE_EXPRESSION);
    expr->u.double_value = value;
    return expr;
}

Expression *alloc_identifier_expression(char *identifier)
{
    Expression *expr = alloc_expression(IDENTIFIER_EXPRESSION);
    expr->u.identifier = identifier;
    return expr;
}

Expression *alloc_bool_expression(bool value)
{
    Expression *expr = alloc_expression(BOOL_EXPRESSION);
    expr->u.boolean_value = value;
    return expr;
}

Expression *alloc_unary_expression(ExpressionKind kind, Expression *unaryExpr)
{
    Expression *expr = alloc_expression(kind);
    expr->u.unary_expression = unaryExpr;
    return expr;
}

Expression *alloc_binary_expression(ExpressionKind kind, Expression *left, Expression *right)
{
    Expression *expr = alloc_expression(kind);
    BinaryExpression *binary = (BinaryExpression *)malloc(sizeof(BinaryExpression));
    binary->left = left;
    binary->right = right;
    expr->u.binary_expression = binary;
    return expr;
}

Expression *alloc_func_call_expression(char *identifier, ArgumentList *argument_list)
{
    Expression *expr = alloc_expression(FUNC_CALL_EXPRESSION);
    FuncCallExpression *func_call_e = (FuncCallExpression *)malloc(sizeof(FuncCallExpression));
    func_call_e->identifier = identifier;
    func_call_e->argument_list = argument_list;
    expr->u.func_call_expression = func_call_e;
    return expr;
}

ArgumentList *chain_argument_list(ArgumentList *list, Expression *expr)
{
    ArgumentList *next = (ArgumentList *)malloc(sizeof(ArgumentList));
    next->expr = expr;
    next->next = NULL;

    if (list == NULL)
    {
        return next;
    }

    list->next = next;
    return list;
}

Statement *alloc_statement(StatementKind kind)
{
    Statement *stmt = (Statement *)malloc(sizeof(Statement));
    stmt->kind = kind;
    return stmt;
}

Statement *alloc_assign_statement(char *variable, Expression *operand)
{
    Statement *stmt = alloc_statement(ASSIGN_STATEMENT);
    AssignStatement *assign_s = (AssignStatement *)malloc(sizeof(AssignStatement));
    assign_s->variable = variable;
    assign_s->operand = operand;
    stmt->u.assign_s = assign_s;
    return stmt;
}

Statement *alloc_if_statement(Expression *condition, Block *then_block, Elseif *elseif_list, Block *else_block)
{
    Statement *stmt = alloc_statement(IF_STATEMENT);
    IfStatement *if_s = (IfStatement *)malloc(sizeof(IfStatement));
    if_s->condition = condition;
    if_s->then_block = then_block;
    if_s->elseif_list = elseif_list;
    if_s->else_block = else_block;
    stmt->u.if_s = if_s;
    return stmt;
}

Statement *alloc_declaration_stmt(char *name, TypeSpecifier *type, Expression *initializer)
{
    Statement *stmt = alloc_statement(DECLARATION_STATEMENT);
    Declaration *decl_s = (Declaration *)malloc(sizeof(Declaration));
    decl_s->name = name;
    decl_s->type = type;
    decl_s->initializer = initializer;
    stmt->u.decl_s = decl_s;
    return stmt;
}

Statement *alloc_return_stmt(Expression *expr)
{
    Statement *stmt = alloc_statement(RETURN_STATEMENT);
    ReturnStatement *return_s = (ReturnStatement *)malloc(sizeof(ReturnStatement));
    return_s->return_value = expr;
    stmt->u.return_s = return_s;
    return stmt;
}

Statement *alloc_block_statement(Block *block)
{
    Statement *stmt = alloc_statement(BLOCK_STATEMENT);
    stmt->u.block_s = block;
    return stmt;
}

StatementList *chain_statement_list(StatementList *list, Statement *statement)
{
    StatementList *stmt_list = (StatementList *)malloc(sizeof(StatementList));
    stmt_list->statement = statement;
    stmt_list->next = NULL;

    if (list == NULL)
    {
        return stmt_list;
    }

    StatementList *pos;
    for (pos = list; pos->next; pos = pos->next)
        ;

    pos->next = stmt_list;
    return list;
}

Elseif *alloc_else_if(Expression *condition, Block *block)
{
    Elseif *elseif = (Elseif *)malloc(sizeof(Elseif));
    elseif->condition = condition;
    elseif->block = block;
    return elseif;
}

Elseif *chain_else_if_list(Elseif *list, Elseif *elseif)
{
    if (list == NULL)
    {
        return elseif;
    }

    Elseif *pos;
    for (pos = list; pos->next; pos = pos->next)
        ;

    pos->next = elseif;
    return list;
}

Block *open_block()
{
    Block *new_block;
    Compiler *compiler = get_current_compiler();
    new_block = alloc_block(NULL);
    new_block->outer_block = compiler->current_block;
    compiler->current_block = new_block;
    return new_block;
}

Block *close_block(Block *block, StatementList *stmt_list)
{
    Compiler *compiler = get_current_compiler();
    block->statement_list = stmt_list;
    compiler->current_block = block->outer_block;
    return block;
}

Block *alloc_block(StatementList *list)
{
    Block *block = (Block *)malloc(sizeof(Block));
    block->statement_list = list;
    return block;
}

TypeSpecifier *alloc_type_specifier(BasicType type, char *identifier)
{
    TypeSpecifier *type_s = (TypeSpecifier *)malloc(sizeof(TypeSpecifier));
    type_s->basic_type = type;
    type_s->identifier = identifier;
    return type_s;
}

ParameterList *alloc_parameter(TypeSpecifier *type, char *identifier)
{
    ParameterList *p = (ParameterList *)malloc(sizeof(ParameterList));
    p->name = identifier;
    p->type = type;
    p->next = NULL;
    return p;
}

ParameterList *chain_parameter(ParameterList *list, ParameterList *parameter)
{
    if (list == NULL)
    {
        return parameter;
    }
    ParameterList *pos;
    for (pos = list; pos->next; pos = pos->next)
        ;
    pos->next = parameter;
    return list;
}

Definition *alloc_definition(DefinitionKind kind)
{
    Definition *definition = (Definition *)malloc(sizeof(Definition));
    definition->kind = kind;
    return definition;
}

Definition *alloc_func_definition(char *name, ParameterList *parameters, TypeSpecifier *return_type, Block *block)
{
    FuncDefinition *func_d = (FuncDefinition *)malloc(sizeof(FuncDefinition));
    func_d->name = name;
    func_d->parameters = parameters;
    func_d->return_type = return_type;
    func_d->block = block;
    func_d->next = NULL;

    Definition *definition = alloc_definition(FUNC_DEFINITION);
    definition->u.func_d = func_d;
    return definition;
}

FuncDefinition *chain_func_definition_list(FuncDefinition *list, FuncDefinition *next)
{
    if (list == NULL)
    {
        return next;
    }

    FuncDefinition *pos;
    for (pos = list; pos->next; pos = pos->next)
        ;

    pos->next = next;
    return list;
}

Definition *alloc_declaration_definition(Statement *declaration_stmt)
{
    Definition *definition = alloc_definition(DECLARATION_DEFINITION);
    definition->u.declaration = declaration_stmt->u.decl_s;
    return definition;
}

DefinitionList *alloc_definition_list(Definition *definition)
{
    DefinitionList *list = (DefinitionList *)malloc(sizeof(DefinitionList));
    list->definition = definition;
    list->next = NULL;
    return list;
}

DefinitionList *chain_definition_list(DefinitionList *list, Definition *definition)
{
    if (list == NULL)
    {
        list->next = alloc_definition_list(definition);
        return list;
    }
    DefinitionList *pos;
    for (pos = list; pos->next; pos = pos->next)
        ;
    pos->next = alloc_definition_list(definition);
    return list;
}
