
include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR})

find_program(BISON_EXECUTABLE bison
    PATHS 
    /usr/bin
    /bin
    /usr/pkg/bin
    /usr/pkg/usr/bin)

find_program(FLEX_EXECUTABLE flex
    PATHS 
    /usr/bin
    /bin
    /usr/pkg/bin
    /usr/pkg/usr/bin)


add_custom_command(
    #TARGET AdminParser 
    #PRE_BUILD
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/parser.h ${CMAKE_CURRENT_BINARY_DIR}/parser.c
    COMMAND ${BISON_EXECUTABLE} -oparser.c parser.y
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/parser.y ${BISON_EXECUTABLE}
    )
add_custom_target(generate_admin_parser DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/parser.c)

add_custom_command(
    #TARGET AdminParser 
    #PRE_BUILD
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/scanner.cc
    COMMAND ${FLEX_EXECUTABLE} -oscanner.cc scanner.ll
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scanner.ll ${FLEX_EXECUTABLE}
    )
add_custom_target(generate_admin_lexer DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/scanner.cc)

set(AdminParser_STAT_SRCS
AdminExecutableTreeNode.cpp
AdminExecutableTreeNode.h
AdminExecutor.cpp
AdminExecutor.h
AdminLexer.h
AdminParser.cpp
AdminParser.h
AdminTreeNode.cpp
AdminTreeNode.h
ShutdownTreeNode.cpp
ShutdownTreeNode.h
scanner.cc)

add_library(AdminParser STATIC ${AdminParser_STAT_SRCS})
add_DEPENDENCIES(AdminParser generate_admin_parser generate_admin_lexer)
