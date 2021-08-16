#ifndef PRIVATE_SHARE_H_INCLUDED
#define PRIVATE_SHARE_H_INCLUDED

#include <stdio.h>
#include <wchar.h>
#include <stdbool.h>
#include "../compiler/summoner.h"

/* wchar.c */
typedef struct
{
    wchar_t *string;
} VWString;

void dkc_vwstr_append_string(VWString *v, wchar_t *str);
void dkc_vwstr_append_character(VWString *v, int ch);

size_t dvm_wcslen(wchar_t *str);
wchar_t *dvm_wcscpy(wchar_t *dest, wchar_t *src);
wchar_t *dvm_wcsncpy(wchar_t *dest, wchar_t *src, size_t n);
int dvm_wcscmp(wchar_t *s1, wchar_t *s2);
wchar_t *dvm_wcscat(wchar_t *s1, wchar_t *s2);
int dvm_mbstowcs_len(const char *src);
void dvm_mbstowcs(const char *src, wchar_t *dest);
int dvm_wcstombs_len(const wchar_t *src);
void dvm_wcstombs_i(const wchar_t *src, char *dest);
char *dvm_wcstombs_alloc(const wchar_t *src);
char dvm_wctochar(wchar_t src);
int dvm_print_wcs(FILE *fp, wchar_t *str);
int dvm_print_wcs_ln(FILE *fp, wchar_t *str);
bool dvm_iswdigit(wchar_t ch);

/* disassemble.c */
int svm_dump_instruction(FILE *fp, SVM_Byte *code, int index);
void svm_disassemble(SVM_Executable *exe);

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

#endif
