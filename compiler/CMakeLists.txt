add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/lex.yy.c
        COMMAND lex -o ${CMAKE_CURRENT_SOURCE_DIR}/lex.yy.c ${CMAKE_CURRENT_SOURCE_DIR}/lex.l
)
add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/y.tab.c
        COMMAND yacc -d -o ${CMAKE_CURRENT_SOURCE_DIR}/y.tab.c ${CMAKE_CURRENT_SOURCE_DIR}/grammar.y
)
add_library(COMPILER lex.yy.c y.tab.c codegen.c disassemble.c create.c error.c fix_tree.c opcode.c string.c compiler.c)
target_include_directories(COMPILER INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
