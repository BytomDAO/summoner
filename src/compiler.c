#include "summoner.h"

static Compiler *st_current_compiler;

Compiler *get_current_compiler()
{
    return st_current_compiler;
}

void set_current_compiler(Compiler *compiler)
{
    st_current_compiler = compiler;
}
