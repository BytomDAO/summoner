#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <limits.h>
#include <stdbool.h>
#include "../include/share.h"
#include "../include/DBG.h"

size_t
dvm_wcslen(wchar_t *str)
{
    return wcslen(str);
}

wchar_t *
dvm_wcscpy(wchar_t *dest, wchar_t *src)
{
    return wcscpy(dest, src);
}

wchar_t *
dvm_wcsncpy(wchar_t *dest, wchar_t *src, size_t n)
{
    return wcsncpy(dest, src, n);
}

int dvm_wcscmp(wchar_t *s1, wchar_t *s2)
{
    return wcscmp(s1, s2);
}

wchar_t *
dvm_wcscat(wchar_t *s1, wchar_t *s2)
{
    return wcscat(s1, s2);
}

int dvm_mbstowcs_len(const char *src)
{
    int src_idx, dest_idx;
    int status;
    mbstate_t ps;

    memset(&ps, 0, sizeof(mbstate_t));
    for (src_idx = dest_idx = 0; src[src_idx] != '\0';)
    {
        status = mbrtowc(NULL, &src[src_idx], MB_LEN_MAX, &ps);
        if (status < 0)
        {
            return status;
        }
        dest_idx++;
        src_idx += status;
    }

    return dest_idx;
}

void dvm_mbstowcs(const char *src, wchar_t *dest)
{
    int src_idx, dest_idx;
    int status;
    mbstate_t ps;

    memset(&ps, 0, sizeof(mbstate_t));
    for (src_idx = dest_idx = 0; src[src_idx] != '\0';)
    {
        status = mbrtowc(&dest[dest_idx], &src[src_idx],
                         MB_LEN_MAX, &ps);
        dest_idx++;
        src_idx += status;
    }
    dest[dest_idx] = L'\0';
}

int dvm_wcstombs_len(const wchar_t *src)
{
    int src_idx, dest_idx;
    int status;
    char dummy[MB_LEN_MAX];
    mbstate_t ps;

    memset(&ps, 0, sizeof(mbstate_t));
    for (src_idx = dest_idx = 0; src[src_idx] != L'\0';)
    {
        status = wcrtomb(dummy, src[src_idx], &ps);
        src_idx++;
        dest_idx += status;
    }

    return dest_idx;
}

void dvm_wcstombs_i(const wchar_t *src, char *dest)
{
    int src_idx, dest_idx;
    int status;
    mbstate_t ps;

    memset(&ps, 0, sizeof(mbstate_t));
    for (src_idx = dest_idx = 0; src[src_idx] != '\0';)
    {
        status = wcrtomb(&dest[dest_idx], src[src_idx], &ps);
        src_idx++;
        dest_idx += status;
    }
    dest[dest_idx] = '\0';
}

char *
dvm_wcstombs_alloc(const wchar_t *src)
{
    int len;
    char *ret;

    len = dvm_wcstombs_len(src);
    ret = malloc(len + 1);
    dvm_wcstombs_i(src, ret);

    return ret;
}

char dvm_wctochar(wchar_t src)
{
    mbstate_t ps;
    int status;
    char dest;

    memset(&ps, 0, sizeof(mbstate_t));
    status = wcrtomb(&dest, src, &ps);
    DBG_assert(status == 1, ("wcrtomb status..%d\n", status));

    return dest;
}

int dvm_print_wcs(FILE *fp, wchar_t *str)
{
    char *tmp;
    int result;

    tmp = dvm_wcstombs_alloc(str);
    result = fprintf(fp, "%s", tmp);
    free(tmp);

    return result;
}

int dvm_print_wcs_ln(FILE *fp, wchar_t *str)
{
    int result;

    result = dvm_print_wcs(fp, str);
    fprintf(fp, "\n");

    return result;
}

bool dvm_iswdigit(wchar_t ch)
{
    return ch >= L'0' && ch <= L'9';
}

static int my_wcslen(wchar_t *str)
{
    if (str == NULL)
    {
        return 0;
    }
    return dvm_wcslen(str);
}

void dkc_vwstr_append_string(VWString *v, wchar_t *str)
{
    int new_size;
    int old_len;

    old_len = my_wcslen(v->string);
    new_size = sizeof(wchar_t) * (old_len + dvm_wcslen(str) + 1);
    v->string = realloc(v->string, new_size);
    dvm_wcscpy(&v->string[old_len], str);
}

void dkc_vwstr_append_character(VWString *v, int ch)
{
    int current_len;

    current_len = my_wcslen(v->string);
    v->string = realloc(v->string, sizeof(wchar_t) * (current_len + 2));
    v->string[current_len] = ch;
    v->string[current_len + 1] = L'\0';
}
