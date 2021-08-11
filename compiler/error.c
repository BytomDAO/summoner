#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "error.h"
#include "summoner.h"
#include "../include/DBG.h"
#include "../include/share.h"

#define LINE_BUF_SIZE (1024)

extern char *yytext;
static ErrorDefinition dkc_error_message_format[] = {
    {"dummy"},
    {"在($(token))附近发生语法错误"},
    {"不正确的字符($(bad_char))"},
    {"函数名重复($(name))"},
    {"不正确的多字节字符。"},
    {"预期外的宽字符串。"},
    {"数组元素不能标识为final。"},
    {"复合赋值运算符不能用于final值"},
    {"函数的参数名重复($(name))。"},
    {"变量名$(name)重复。"},
    {"找不到变量或函数$(name)。"},
    {"$(name)是函数名，但没有函数调用的()。"},
    {"不能强制转型为派生类型。"},
    {"不能将$(src)转型为$(dest)。"},
    {"算数运算符的操作数类型不正确。"},
    {"比较运算符的操作数类型不正确。"},
    {"逻辑and/or运算符的操作数类型不正确。"},
    {"减法运算符的操作数类型不正确。"},
    {"逻辑非运算符的操作数类型不正确。"},
    {"自增/自减运算符的操作数类型不正确。"},
    {"函数调用运算符的操作数不是函数名。"},
    {"找不到函数$(name)。"},
    {"函数的参数数量错误。"},
    {"赋值运算符的左边不是一个左边值。"},
    {"标签$(label)不存在。"},
    {"数组字面量必须至少有一个元素"},
    {"下标运算符[]的左边不是数组类型"},
    {"数组的下标不是int。"},
    {"数组的大小不是int。"},
    {"整数值不能被0除。"},
    {"package名称过长"},
    {"被require的文件不存在($(file))"},
    {"require时发生错误($(status))。"},
    {"同一个源文件中重复require了同一个包。"},
    {"rename后的名称必须指定package。"},
    {"重复声明了abstract。"},
    {"重复声明了访问修饰符。"},
    {"重复声明了override。"},
    {"重复声明了virtual。"},
    {"该类型不能使用成员运算符。"},
    {"在类型$(class_name)中不存在成员$(member_name)。"},
    {"成员$(member_name)是private的，不能访问。"},
    {"没有实现abstract方法。"},
    {"必须实现非abstract方法。"},
    {"不能继承多个类。"},
    {"Diksam中不能继承非abstract的类"
     "(类$(name)不是abstract类)。"},
    {"不能对abstract类($(name))使用new。"},
    {"void类型的函数不能有返回值。"},
    {"没有找到类$(name)。"},
    {"成员$(member_name)被声明为构造方法。并不是方法。"},
    {"用来new的方法$(member_name)并不是构造方法。"},
    {"不能调用字段$(member_name)"},
    {"尝试为方法$(member_name)赋值。"},
    {"不能覆盖非virtual方法$(name)。"},
    {"覆盖方法时必须使用override关键字($(name))。"},
    {"在abstract类中，存在非abstract方法$(method_name)。"},
    {"在没有超类的类中使用了super。"},
    {"方法调用以外不能使用super。"},
    {"不能引用super的字段。"},
    {"$(name)是字段，不能覆盖。"},
    {"重复的字段名$(name)。"},
    {"数组中没有$(name)方法。"},
    {"数组中没有$(name)方法。"},
    {"instanceof的操作数必须是引用类型。"},
    {"instanceof的右边的类型必须是引用类型。"},
    {"instanceof的目标必须是类。"},
    {"instanceof语句一直为真。"},
    {"instanceof语句一直为假。"},
    {"因为Diksam的接口间没有父子关系"
     "instanceof语句一直为假。"},
    {"向下转型的源类型必须是类。"},
    {"向下转型的目标类型必须是类。"},
    {"不需要进行向下转型。"},
    {"尝试将父类转换为子类。"},
    {"尝试转换没有继承关系的类。"},
    {"因为Diksam的接口间没有父子关系"
     "不能向下转型。"},
    {"不能catch类以外的类型。"},
    {"catch到的类$(class_name)不是Exception的子类。"},
    {"不能throw类以外的类型。"},
    {"throw的类$(class_name)不是Exception的子类。"},
    {"catch之外不能出现没有操作数的throw。"},
    {"$(statement_name)语句不能卸载finally块中。"},
    {"找不到throws中声明的$(name)。"},
    {"throws中声明的$(name)不是Exception的子类。"},
    {"在throws块中声明的异常$(name)，必须在内部catch。"},
    {"不能require文件本身。"},
    {"if语句的条件表达式不是boolean型。"},
    {"while语句的条件表达式不是boolean型。"},
    {"for语句的条件表达式不是boolean型。"},
    {"do while语句的条件表达式不是boolean型。"},
    {"case块的表达式的类型不一致"},
    {"$(name)为final变量。不能赋值。"},
    {"$(name)为final字段。不能赋值。"},
    {"final变量$(name)没有初始化。"},
    {"被覆盖的方法$(name)的访问修饰符必须比超类的更严格。"},
    {"方法或函数$(name)的参数数量错误。"},
    {"方法或函数$(func_name)的第$(index)个参数"
     "$(param_name)的类型错误。"},
    {"方法或函数$(name)的返回值类型错误。"},
    {"方法或函数$(func_name)描述的throws中含有不恰当的异常$(exception_name)。"},
    {"不能直接调用构造方法。"},
    {"找不到类型名$(name)。"},
    {"枚举类型$(type_name)中没有$(enumerator_name)。"},
    {"Diksam的接口之间不能继承（至今为止）。"},
    {"不能从包外访问成员$(member_name)。"},
    {"不能从包外访问类$(class_name)。"},
    {"不能在类外使用this。"},
    {"不能在类外使用super。"},
    {"~运算符的操作数必须是int。"},
    {"双目bite运算符的操作数必须是int或者boolean。"},
    {"在C样式的注释中终止了文件。"},
    {"在字符串字面量中终止了文件。"},
    {"字符字面量中包含了2个以上的字符。"},
    {"dummy"}
};

typedef struct
{
    MessageArgumentType type;
    char *name;
    union
    {
        int int_val;
        double double_val;
        char *string_val;
        void *pointer_val;
        int character_val;
    } u;
} MessageArgument;

static void
create_message_argument(MessageArgument *arg, va_list ap)
{
    int index = 0;
    MessageArgumentType type;

    while ((type = va_arg(ap, MessageArgumentType)) != MESSAGE_ARGUMENT_END)
    {
        arg[index].type = type;
        arg[index].name = va_arg(ap, char *);
        switch (type)
        {
        case INT_MESSAGE_ARGUMENT:
            arg[index].u.int_val = va_arg(ap, int);
            break;
        case DOUBLE_MESSAGE_ARGUMENT:
            arg[index].u.double_val = va_arg(ap, double);
            break;
        case STRING_MESSAGE_ARGUMENT:
            arg[index].u.string_val = va_arg(ap, char *);
            break;
        case POINTER_MESSAGE_ARGUMENT:
            arg[index].u.pointer_val = va_arg(ap, void *);
            break;
        case CHARACTER_MESSAGE_ARGUMENT:
            arg[index].u.character_val = va_arg(ap, int);
            break;
        case MESSAGE_ARGUMENT_END:
            assert(0);
            break;
        default:
            assert(0);
        }
        index++;
        assert(index < MESSAGE_ARGUMENT_MAX);
    }
}

static void
search_argument(MessageArgument *arg_list,
                char *arg_name, MessageArgument *arg)
{
    int i;

    for (i = 0; arg_list[i].type != MESSAGE_ARGUMENT_END; i++)
    {
        if (!strcmp(arg_list[i].name, arg_name))
        {
            *arg = arg_list[i];
            return;
        }
    }
    assert(0);
}

wchar_t *
mbstowcs_alloc(int line_number, const char *src)
{
    int len;
    wchar_t *ret;

    len = dvm_mbstowcs_len(src);
    if (len < 0)
    {
        return NULL;
        compile_error(line_number,
                      BAD_MULTIBYTE_CHARACTER_ERR,
                      MESSAGE_ARGUMENT_END);
    }
    ret = malloc(sizeof(wchar_t) * (len + 1));
    dvm_mbstowcs(src, ret);

    return ret;
}

static void
format_message(int line_number, ErrorDefinition *format, VWString *v,
               va_list ap)
{
    int i;
    char buf[LINE_BUF_SIZE];
    wchar_t wc_buf[LINE_BUF_SIZE];
    int arg_name_index;
    char arg_name[LINE_BUF_SIZE];
    MessageArgument arg[MESSAGE_ARGUMENT_MAX];
    MessageArgument cur_arg;
    wchar_t *wc_format;

    create_message_argument(arg, ap);

    wc_format = mbstowcs_alloc(line_number, format->format);
    DBG_assert(wc_format != NULL, ("wc_format is null.\n"));

    for (i = 0; wc_format[i] != L'\0'; i++)
    {
        if (wc_format[i] != L'$')
        {
            dkc_vwstr_append_character(v, wc_format[i]);
            continue;
        }
        assert(wc_format[i + 1] == L'(');
        i += 2;
        for (arg_name_index = 0; wc_format[i] != L')';
             arg_name_index++, i++)
        {
            arg_name[arg_name_index] = dvm_wctochar(wc_format[i]);
        }
        arg_name[arg_name_index] = '\0';
        assert(wc_format[i] == L')');

        search_argument(arg, arg_name, &cur_arg);
        switch (cur_arg.type)
        {
        case INT_MESSAGE_ARGUMENT:
            sprintf(buf, "%d", cur_arg.u.int_val);
            dvm_mbstowcs(buf, wc_buf);
            dkc_vwstr_append_string(v, wc_buf);
            break;
        case DOUBLE_MESSAGE_ARGUMENT:
            sprintf(buf, "%f", cur_arg.u.double_val);
            dvm_mbstowcs(buf, wc_buf);
            dkc_vwstr_append_string(v, wc_buf);
            break;
        case STRING_MESSAGE_ARGUMENT:
            dvm_mbstowcs(cur_arg.u.string_val, wc_buf);
            dkc_vwstr_append_string(v, wc_buf);
            break;
        case POINTER_MESSAGE_ARGUMENT:
            sprintf(buf, "%p", cur_arg.u.pointer_val);
            dvm_mbstowcs(buf, wc_buf);
            dkc_vwstr_append_string(v, wc_buf);
            break;
        case CHARACTER_MESSAGE_ARGUMENT:
            sprintf(buf, "%c", cur_arg.u.character_val);
            dvm_mbstowcs(buf, wc_buf);
            dkc_vwstr_append_string(v, wc_buf);
            break;
        case MESSAGE_ARGUMENT_END:
            assert(0);
            break;
        default:
            assert(0);
        }
    }
    free(wc_format);
}

static void
self_check()
{
    if (strcmp(dkc_error_message_format[0].format, "dummy") != 0)
    {
        DBG_panic(("compile error message format error.\n"));
    }
    if (strcmp(dkc_error_message_format
                   [COMPILE_ERROR_COUNT_PLUS_1]
                       .format,
               "dummy") != 0)
    {
        DBG_panic(("compile error message format error. "
                   "COMPILE_ERROR_COUNT_PLUS_1..%d\n",
                   COMPILE_ERROR_COUNT_PLUS_1));
    }
}

void compile_error(int line_number, CompileError id, ...)
{
    va_list ap;
    VWString message;

    self_check();
    va_start(ap, id);

    message.string = NULL;
    format_message(line_number,
                   &dkc_error_message_format[id],
                   &message, ap);
    fprintf(stderr, "%3d:", line_number);
    dvm_print_wcs_ln(stderr, message.string);
    va_end(ap);

    exit(1);
}

int yyerror(char const *str)
{
    compile_error(get_current_compiler()->current_line_number,
                  PARSE_ERR,
                  STRING_MESSAGE_ARGUMENT, "token", yytext,
                  MESSAGE_ARGUMENT_END);

    return 0;
}
