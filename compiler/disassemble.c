#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "summoner.h"
#include "error.h"
#include "../include/DBG.h"
#include "../include/share.h"
#include "../include/SVM_code.h"

extern OpcodeInfo svm_opcode_info[];

static void dump_type(SVM_Executable *exe, TypeSpecifier *type);

static void
dump_parameter_list(SVM_Executable *exe,
                    int parameter_count, SVM_Variable *parameter_list)
{
    int i;

    printf("(");
    for (i = 0; i < parameter_count; i++) {
        printf(" %s", parameter_list[i].name);
        if (i < parameter_count-1) {
            printf(", ");
        }
    }
    printf(")");
}

static void
dump_type(SVM_Executable *exe, TypeSpecifier *type)
{
    switch (type->basic_type) {
    case VOID_TYPE:
        printf("void ");
        break;
    case BOOLEAN_TYPE:
        printf("boolean ");
        break;
    case INT_TYPE:
        printf("int ");
        break;
    case DOUBLE_TYPE:
        printf("double ");
        break;
    case STRING_TYPE:
        printf("string ");
        break;
    case ASSET_TYPE:
        printf("asset ");
        break;
    case HASH_TYPE:
        printf("hash ");
        break;
    case AMOUNT_TYPE:
        printf("amount ");
        break;
    case PUBKEY_TYPE:
        printf("string ");
        break;
    case SIG_TYPE:
        printf("sig ");
        break;
    case HEX_TYPE:
        printf("hex ");
        break;
    case STRUCT_TYPE:
        printf("struct ");
        break;
    default:
        DBG_assert(0, ("basic_type..%d\n", type->basic_type));
    }
}

static void
dump_variable(SVM_Executable *exe, int global_variable_count,
              SVM_Variable *global_variable)
{
    int i;

    printf("** global variable section *********************************\n");
    for (i = 0; i < global_variable_count; i++) {
        printf("%5d:", i);
        dump_type(exe, global_variable[i].type);
        printf(" %s\n", global_variable[i].name);
    }
}

int
svm_dump_instruction(FILE *fp, SVM_Byte *code, int index)
{
    OpcodeInfo *info;
    int value;
    int i;

    info = &svm_opcode_info[code[index]];
    fprintf(fp, "%4d %s ", index, info->mnemonic);
    index++;

    return index;
}

static void
dump_opcode(int code_size, SVM_Byte *code)
{
    int index;

    for (index = 0; index < code_size; ) {
        if (code[index] >= OP_DATA_1 && code[index] <= OP_DATA_75) {
            index = svm_dump_instruction(stdout, code, index);
            printf("\n");

            fprintf(stdout, "%4d ", index);
            int len = code[index-1] - OP_DATA_1 + 1;
            for (int i = 0; i < len; i++) {
                fprintf(stdout, "%c", code[index + i]);
            }
            index += len;   
        } else {
            index = svm_dump_instruction(stdout, code, index);
        }
        printf("\n");
    }
}

static void
dump_line_number(int line_number_size, SVM_LineNumber *line_number)
{
    int i;

    printf("*** line number ***\n");
    for (i = 0; i < line_number_size; i++) {
        printf("%5d: from %5d size %5d\n",
               line_number[i].line_number,
               line_number[i].start_pc,
               line_number[i].pc_count);
    }
}

static void
dump_function(SVM_Executable *exe, int function_count, SVM_Function *function)
{
    int i;

    printf("** function section ****************************************\n");
    for (i = 0; i < function_count; i++) {
        printf("*** [%d] %s ***\n", i, function[i].name);

        dump_type(exe, function[i].type);
        printf(" %s ", function[i].name);
        dump_parameter_list(exe,
                            function[i].parameter_count,
                            function[i].parameter);
        printf("\n");

        if (function[i].code_block.code_size > 0) {
            dump_opcode(function[i].code_block.code_size,
                        function[i].code_block.code);
            dump_line_number(function[i].code_block.line_number_size,
                                function[i].code_block.line_number);
        }
        
        printf("*** end of %s ***\n", function[i].name);
    }
}

static void
dump_types(SVM_Executable *exe, int type_count, TypeSpecifier *types)
{
    int i;

    printf("** type section ********************************************\n");
    for (i = 0; i < type_count; i++) {
        printf("%5d:", i);
        dump_type(exe, &types[i]);
        printf("\n");
    }
}

void
svm_disassemble(SVM_Executable *exe)
{
    dump_variable(exe, exe->global_variable_count, exe->global_variable);
    dump_function(exe, exe->function_count, exe->function);
    dump_types(exe, exe->type_specifier_count, exe->type_specifier);
    printf("** toplevel ********************\n");
    dump_opcode(exe->top_level.code_size, exe->top_level.code);
    dump_line_number(exe->top_level.line_number_size,
                     exe->top_level.line_number);
}
