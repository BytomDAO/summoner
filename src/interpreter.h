#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_

#include "summoner.h"

ExprValue evalExpression(Expression *expr);
void printExprValue(ExprValue);

#endif