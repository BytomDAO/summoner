
#include <stdio.h>
#include <stdlib.h>
#include "eval.h"
#include <iostream>

void print_expr_value(ExprValue val);

ExprValue eval_int_binary_expression(ExpressionKind type, int left, int right)
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

ExprValue eval_double_binary_expression(ExpressionKind kind, double left, double right)
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

ExprValue eval_bool_binary_expression(ExpressionKind type, bool left, bool right)
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

ExprValue Interpreter::eval_binary_expression(ExpressionKind kind, BinaryExpression *binaryExpression)
{
    ExprValue leftVal = this->eval_expression(binaryExpression->left);
    ExprValue rightVal = this->eval_expression(binaryExpression->right);

    if (leftVal.type == EXPR_INT_VALUE && rightVal.type == EXPR_INT_VALUE)
    {
        return eval_int_binary_expression(kind, leftVal.u.int_value, rightVal.u.int_value);
    }

    if (leftVal.type == EXPR_INT_VALUE && rightVal.type == EXPR_DOUBLE_VALUE)
    {
        return eval_double_binary_expression(kind, leftVal.u.int_value, rightVal.u.double_value);
    }

    if (leftVal.type == EXPR_DOUBLE_VALUE && rightVal.type == EXPR_INT_VALUE)
    {
        return eval_double_binary_expression(kind, leftVal.u.double_value, rightVal.u.int_value);
    }

    if (leftVal.type == EXPR_DOUBLE_VALUE && rightVal.type == EXPR_DOUBLE_VALUE)
    {
        return eval_double_binary_expression(kind, leftVal.u.double_value, rightVal.u.double_value);
    }

    if (leftVal.type == EXPR_BOOL_VALUE && rightVal.type == EXPR_BOOL_VALUE)
    {
        return eval_bool_binary_expression(kind, leftVal.u.boolean_value, rightVal.u.boolean_value);
    }

    printf("invalid expression type, left:%d, right:%d", leftVal.type, rightVal.type);
    exit(1);
}

ExprValue Interpreter::eval_expression(Expression *expr)
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
        v = this->eval_expression(expr->u.unary_expression);
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
        v = this->eval_expression(expr->u.unary_expression);
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
        return this->eval_binary_expression(expr->kind, expr->u.binary_expression);
    case FUNC_CALL_EXPRESSION:
    {
        FuncCallExpression *func_call_expr = expr->u.func_call_expression;
        FuncDefinition *func_d = this->find_func_definition(func_call_expr->identifier);
        if (func_d == nullptr)
        {
            cout << "func \"" << func_call_expr->identifier << "\" is undefined" << endl;
            exit(1);
        }
        return *(this->eval_func(func_d, func_call_expr->argument_list));
    }
    case IDENTIFIER_EXPRESSION:
        return this->current_scope->find_identifier(expr->u.identifier)->get_value();
    default:
        printf("invalid expression type when eval expression:%d\n", expr->kind);
        exit(1);
    }
}

void Interpreter::exec()
{
    for (DeclarationList *pos = this->declaration_list; pos != nullptr; pos = pos->next)
    {
        Declaration *declaration = pos->declaration;
        this->current_scope->put_identifier(new Identifier(declaration->name, this->eval_expression(declaration->initializer)));
    }

    FuncDefinition *func_d = this->find_func_definition("main");
    if (func_d == nullptr)
    {
        cout << "fail to find main func" << endl;
        exit(1);
    }
    this->eval_func(func_d, nullptr);
}

FuncDefinition *Interpreter::find_func_definition(const char *name)
{
    FuncDefinition *list = this->func_definition_list;
    for (; list != nullptr; list = list->next)
    {
        if (!strcmp(list->name, name))
        {
            return list;
        }
    }
    return nullptr;
}

ExprValue *Interpreter::eval_func(FuncDefinition *fd, ArgumentList *arg_list)
{
    ParameterList *params = fd->parameters;
    vector<Identifier *> identifiers;
    for (; params != nullptr && arg_list != nullptr; arg_list = arg_list->next, params = params->next)
    {
        ExprValue arg_val = this->eval_expression(arg_list->expr);
        identifiers.push_back(new Identifier(params->name, arg_val));
    }

    StmtResult result = this->eval_block(fd->block, identifiers);
    if (result.type == RETURN_STATEMENT_RESULT)
    {
        return new ExprValue(result.u.return_value);
    }
    return nullptr;
}

StmtResult Interpreter::eval_block(Block *block, vector<Identifier *> identifiers)
{
    Scope *parent = this->current_scope;
    Scope *scope = new Scope(parent);
    this->current_scope = scope;

    for (size_t i = 0; i < identifiers.size(); i++)
    {
        scope->put_identifier(identifiers[i]);
    }

    StmtResult result;
    for (StatementList *stmt = block->statement_list; stmt != nullptr; stmt = stmt->next)
    {
        result = this->eval_stmt(stmt->statement);
        if (result.type != NORMAL_STATEMENT_RESULT)
        {
            break;
        }
    }

    delete scope;
    this->current_scope = parent;
    return result;
}

StmtResult Interpreter::eval_stmt(Statement *stmt)
{
    StmtResult result = {NORMAL_STATEMENT_RESULT};
    switch (stmt->kind)
    {
    case BLOCK_STATEMENT:
        return this->eval_block(stmt->u.block_s, vector<Identifier *>());
    case ASSIGN_STATEMENT:
    {
        AssignStatement *assign_s = stmt->u.assign_s;
        Identifier *identifier = this->current_scope->find_identifier(assign_s->variable);
        if (identifier == nullptr)
        {
            cout << "\"" << assign_s->variable << "\" is undefined" << endl;
            exit(1);
        }

        identifier->set_value(this->eval_expression(assign_s->operand));
        return result;
    }
    case IF_STATEMENT:
    {
        IfStatement *if_s = stmt->u.if_s;
        bool cond = this->eval_expression(if_s->condition).u.boolean_value;
        if (cond)
        {
            return this->eval_block(if_s->then_block, vector<Identifier *>());
        }
        else
        {
            for (Elseif *elseif = if_s->elseif_list; elseif != nullptr; elseif = elseif->next)
            {
                bool cond = this->eval_expression(elseif->condition).u.boolean_value;
                if (cond)
                {
                    return this->eval_block(elseif->block, vector<Identifier *>());
                }
            }

            if (if_s->else_block != nullptr)
            {
                return this->eval_block(if_s->else_block, vector<Identifier *>());
            }
        }
        return result;
    }
    case RETURN_STATEMENT:
    {
        result.type = RETURN_STATEMENT_RESULT;
        result.u.return_value = this->eval_expression(stmt->u.expr_s);
        return result;
    }
    case EXPRESSION_STATEMENT:
    {
        print_expr_value(this->eval_expression(stmt->u.expr_s));
        return result;
    }
    case DECLARATION_STATEMENT:
    {
        Declaration *declaration = stmt->u.decl_s;
        ExprValue init_value;
        if (declaration->type != nullptr)
        {
            switch (declaration->type->basic_type)
            {
            case BOOLEAN_TYPE:
                init_value.type = EXPR_BOOL_VALUE;
                init_value.u.boolean_value = false;
                break;
            case INT_TYPE:
                init_value.type = EXPR_INT_VALUE;
                init_value.u.int_value = 0;
                break;
            case DOUBLE_TYPE:
                init_value.type = EXPR_DOUBLE_VALUE;
                init_value.u.double_value = 0.0;
                break;
            default:
                break;
            }
        }

        if (declaration->initializer != nullptr)
        {
            init_value = this->eval_expression(declaration->initializer);
        }
        this->current_scope->put_identifier(new Identifier(declaration->name, init_value));
        return result;
    }
    default:
        cout << "invalid statement kind:" << stmt->kind << endl;
        exit(1);
    }
}

void print_expr_value(ExprValue val)
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
