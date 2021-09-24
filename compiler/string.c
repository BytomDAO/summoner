
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int STRING_ALLOC_SIZE = 128;

static char *st_string_literal_buffer = NULL;
static int st_string_literal_buffer_size = 0;
static int st_string_literal_buffer_alloc_size = 0;

void open_string_literal(void)
{
    st_string_literal_buffer_size = 0;
    st_string_literal_buffer = (char *) malloc(STRING_ALLOC_SIZE);
    st_string_literal_buffer_alloc_size = STRING_ALLOC_SIZE;
}

void add_string_literal(char letter)
{
    if (st_string_literal_buffer_size == st_string_literal_buffer_alloc_size)
    {
        st_string_literal_buffer_alloc_size += STRING_ALLOC_SIZE;
        st_string_literal_buffer = (char *) realloc(st_string_literal_buffer,
                                           st_string_literal_buffer_alloc_size);
    }
    st_string_literal_buffer[st_string_literal_buffer_size] = letter;
    st_string_literal_buffer_size++;
}

void reset_string_literal_buffer()
{
    free(st_string_literal_buffer);
    st_string_literal_buffer = NULL;
    st_string_literal_buffer_size = 0;
    st_string_literal_buffer_alloc_size = 0;
}

char *close_string_literal(void)
{
    st_string_literal_buffer[st_string_literal_buffer_size] = 0;
    char *p = (char *) malloc(st_string_literal_buffer_size + 1);
    strcpy(p, st_string_literal_buffer);
    reset_string_literal_buffer();
    return p;
}
