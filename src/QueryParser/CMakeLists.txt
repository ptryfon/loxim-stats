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
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/parser.h ${CMAKE_CURRENT_BINARY_DIR}/parser.c
    COMMAND ${BISON_EXECUTABLE} -oparser.c parser.y
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/parser.y ${BISON_EXECUTABLE}
    )
add_custom_target(generate_query_parser DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/parser.c)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/scanner.cc
    COMMAND ${FLEX_EXECUTABLE} -oscanner.cc scanner.ll
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scanner.ll ${FLEX_EXECUTABLE}
    )
add_custom_target(generate_query_lexer DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/scanner.cc)

set(QUERYPARSERDIR "../QueryParser")

set(QueryParser_SRCS
	${QUERYPARSERDIR}/AuxRmver.cpp
	${QUERYPARSERDIR}/AuxRmver.h
	${QUERYPARSERDIR}/ClassNames.h
	${QUERYPARSERDIR}/DataRead.cpp
	${QUERYPARSERDIR}/DataRead.h
	${QUERYPARSERDIR}/DeathRmver.cpp
	${QUERYPARSERDIR}/DeathRmver.h
	${QUERYPARSERDIR}/Deb.cpp
	${QUERYPARSERDIR}/Deb.h
	${QUERYPARSERDIR}/IndexNode.h
	${QUERYPARSERDIR}/JoinRpcer.cpp
	${QUERYPARSERDIR}/JoinRpcer.h
	${QUERYPARSERDIR}/Optimizer.cpp
	${QUERYPARSERDIR}/Optimizer.h
	${QUERYPARSERDIR}/ParserWrapper.cpp
	${QUERYPARSERDIR}/ParserWrapper.h
	${QUERYPARSERDIR}/Privilige.cpp
	${QUERYPARSERDIR}/Privilige.h
	${QUERYPARSERDIR}/QueryLexer.h
	${QUERYPARSERDIR}/QueryParser.cpp
	${QUERYPARSERDIR}/QueryParser.h
	${QUERYPARSERDIR}/Stack.cpp
	${QUERYPARSERDIR}/Stack.h
	${QUERYPARSERDIR}/TreeNode.cpp
	${QUERYPARSERDIR}/TreeNode.h
	#${QUERYPARSERDIR}/scanner.cc 
	CACHE INTERNAL "" )


#add_DEPENDENCIES(QueryParser generate_query_parser generate_query_lexer)
