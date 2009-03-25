
include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR} ${INCDIR})

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
    OUTPUT ${CMAKE_CURRNET_BINARY_DIR}/parser.h ${CMAKE_CURRENT_BINARY_DIR}/parser.c
    COMMAND ${BISON_EXECUTABLE} -oparser.c parser.y
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/parser.y
    )
add_custom_target(generate_query_parser DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/parser.c)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/scanner.cc
    COMMAND ${FLEX_EXECUTABLE} -oscanner.cc scanner.ll
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scanner.ll
    )
add_custom_target(generate_query_lexer DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/scanner.cc)



set(QueryParser_STAT_SRCS
AuxRmver.cpp
AuxRmver.h
ClassNames.h
DataRead.cpp
DataRead.h
DeathRmver.cpp
DeathRmver.h
Deb.cpp
Deb.h
IndexNode.h
JoinRpcer.cpp
JoinRpcer.h
Optimizer.cpp
Optimizer.h
ParserWrapper.cpp
ParserWrapper.h
Privilige.cpp
Privilige.h
QueryLexer.h
QueryParser.cpp
QueryParser.h
Stack.cpp
Stack.h
TreeNode.cpp
TreeNode.h
scanner.cc)

add_library(QueryParser STATIC ${QueryParser_STAT_SRCS})

add_DEPENDENCIES(QueryParser generate_query_parser generate_query_lexer)
