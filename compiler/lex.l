%{
#include <stdio.h>
#include <string.h>
#include "y.tab.h"
#include "compiler/summoner.h"

void lex_err(char *str) {
    fprintf(stderr, "lexical error:%s\n", str);
    exit(1);
}
%}

%x COMMENT STRING_STATE STRING_ESCAPE RAW_STRING_STATE
%%

"//"         BEGIN(COMMENT);
<COMMENT>\n  { BEGIN(0); return '\n'; }
<COMMENT>.   ; // eat anything in comment

` { 
    open_string_literal(); 
    BEGIN(RAW_STRING_STATE);
}
<RAW_STRING_STATE>` {
  yylval.str_value = close_string_literal();
  BEGIN(0);
  return STRING_LITERAL;
}
<RAW_STRING_STATE>\n add_string_literal('\n');
<RAW_STRING_STATE>.  add_string_literal(yytext[0]);

\" { 
    open_string_literal(); 
    BEGIN(STRING_STATE); 
}

<STRING_STATE>\"  {
  yylval.str_value = close_string_literal();
  BEGIN(0);
  return STRING_LITERAL;
}
<STRING_STATE>\\   BEGIN(STRING_ESCAPE);
<STRING_STATE>\n   lex_err("new line in string");
<STRING_STATE>.    add_string_literal(yytext[0]);

<STRING_ESCAPE>\" { add_string_literal('"');  BEGIN(STRING_STATE); }
<STRING_ESCAPE>b  { add_string_literal('\b'); BEGIN(STRING_STATE); }
<STRING_ESCAPE>f  { add_string_literal('\f'); BEGIN(STRING_STATE); }
<STRING_ESCAPE>n  { add_string_literal('\n'); BEGIN(STRING_STATE); }
<STRING_ESCAPE>r  { add_string_literal('\r'); BEGIN(STRING_STATE); }
<STRING_ESCAPE>t  { add_string_literal('\t'); BEGIN(STRING_STATE); }
<STRING_ESCAPE>v  { add_string_literal('\v'); BEGIN(STRING_STATE); }
<STRING_ESCAPE>\\ { add_string_literal('\\'); BEGIN(STRING_STATE); }
<STRING_ESCAPE>.  lex_err("unknow escape");

[+\-*/\(\)<>!{}\n=,] {
    return *yytext;
}

":="       return DECL_ASSIGN;
"+="       return ADD_ASSIGN;
"-="       return SUB_ASSIGN;
"*="       return MUL_ASSIGN;
"/="       return DIV_ASSIGN;
">="       return GE;
"<="       return LE;
"=="       return EQ;
"!="       return NE;
"&&"       return AND;
"||"       return OR;
"if"       return IF;
"else"     return ELSE;
"for"      return FOR;
"nil"      return NIL;
"var"      return VAR;
"const"    return CONST;
"func"     return FUNCTION;
"return"   return RETURN;
"break"    return BREAK;
"continue" return CONTINUE;
"bool"     return BOOL_T;
"int"      return INT_T;
"double"   return DOUBLE_T;
"string"   return STRING_T;
"asset"    return ASSET_T;
"hash"     return HASH_T;
"amount"   return AMOUNT_T;
"pubkey"   return PUBKEY_T;
"sig"      return SIG_T;
"hex"      return HEX_T;

"true"|"false" {
    yylval.int_value = strcmp(yytext, "false");
    return BOOL_LITERAL;
}

[0-9]+\.[0-9]+ {
    double val;
    sscanf(yytext, "%lf", &val);
    yylval.double_value = val;
    return DOUBLE_LITERAL;
}

([1-9][0-9]*)|0 {
    int val;
    sscanf(yytext, "%d", &val);
    yylval.int_value = val;
    return INT_LITERAL;
}

[A-Za-z_][A-Za-z_0-9]* {
  char *new_str = (char *)malloc(strlen(yytext) + 1);
  strcpy(new_str, yytext);
  yylval.identifier = new_str;
  return IDENTIFIER;
}

[ \t] ;

. { lex_err(yytext); }
%%

int yywrap(void) {
    return 1;
}
