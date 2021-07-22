#include "summoner.h"
#include <stdlib.h>

static Compiler *st_current_compiler;

Compiler *create_compiler()
{
    Compiler *compiler = malloc(sizeof(Compiler));
    return compiler;
}

Compiler *get_current_compiler()
{
    return st_current_compiler;
}

void set_current_compiler(Compiler *compiler)
{
    st_current_compiler = compiler;
}

void add_definitions_to_compiler(DefinitionList *definitions)
{
}