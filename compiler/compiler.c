#include "summoner.h"
#include <stdlib.h>

static Compiler *st_current_compiler;

Compiler *create_compiler()
{
    Compiler *compiler = (Compiler *)malloc(sizeof(Compiler));
    compiler->func_definition_list = (FuncDefinition *)malloc(sizeof(FuncDefinition));
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
    Compiler *compiler = get_current_compiler();
    for (DefinitionList *pos = definitions; pos != NULL; pos = pos->next)
    {
        switch (pos->definition->kind)
        {
        case FUNC_DEFINITION:
            compiler->func_definition_list = chain_func_definition_list(
                compiler->func_definition_list, pos->definition->u.func_d);
            break;
        default:
            break;
        }
    }
}
