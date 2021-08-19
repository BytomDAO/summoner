#include <string.h>
#include "summoner.h"

Declaration *search_declaration_in_current_block(char *identifier, Block *block)
{
    if (!block)
    {
        return NULL;
    }

    Declaration *d_pos;
    for (d_pos = block->declaration_list; d_pos; d_pos = d_pos->next)
    {
        if (!strcmp(identifier, d_pos->name))
        {
            return d_pos;
        }
    }
    return NULL;
}

Declaration *search_declaration(char *identifier, Block *block)
{
    Block *b_pos;
    Declaration *d_pos;
    Compiler *compiler;

    for (b_pos = block; b_pos; b_pos = b_pos->outer_block)
    {
        for (d_pos = b_pos->declaration_list; d_pos; d_pos = d_pos->next)
        {
            if (!strcmp(identifier, d_pos->name))
            {
                return d_pos;
            }
        }
    }

    compiler = get_current_compiler();
    for (d_pos = compiler->declaration_list; d_pos; d_pos = d_pos->next)
    {
        if (!strcmp(identifier, d_pos->name))
        {
            return d_pos;
        }
    }
    return NULL;
}

FuncDefinition *search_function(char *name)
{
    Compiler *compiler;
    FuncDefinition *func_pos;

    compiler = get_current_compiler();
    for (func_pos = compiler->func_definition_list; func_pos; func_pos = func_pos->next) {
        if (!strcmp(func_pos->name, name)) {
            return func_pos;
        }
    }
    return NULL;
}
