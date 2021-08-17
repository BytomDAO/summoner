cmake_minimum_required(VERSION 3.19)
project(summoner)

set(CMAKE_C_STANDARD 99)

add_subdirectory(compiler)
add_subdirectory(debug)
add_subdirectory(share)
add_subdirectory(builtin)
list(APPEND EXTRA_LIBS COMPILER DEBUG SHARE BUILTIN)

add_executable(summoner main/main.c)

target_link_libraries(summoner PUBLIC ${EXTRA_LIBS})
