set(ADMIN_PARSER_DIR "../AdminParser" CACHE INTERNAL "" )

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
add_custom_target(generate_admin_parser DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/parser.c)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/scanner.cc
    COMMAND ${FLEX_EXECUTABLE} -oscanner.cc scanner.ll
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scanner.ll ${FLEX_EXECUTABLE}
    )
add_custom_target(generate_admin_lexer DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/scanner.cc)




set(AdminParser_SRCS
	${ADMIN_PARSER_DIR}/AdminExecutableTreeNode.cpp
	${ADMIN_PARSER_DIR}/AdminExecutableTreeNode.h
	${ADMIN_PARSER_DIR}/AdminExecutor.cpp
	${ADMIN_PARSER_DIR}/AdminExecutor.h
	${ADMIN_PARSER_DIR}/AdminLexer.h
	${ADMIN_PARSER_DIR}/AdminParser.cpp
	${ADMIN_PARSER_DIR}/AdminParser.h
	${ADMIN_PARSER_DIR}/AdminTreeNode.cpp
	${ADMIN_PARSER_DIR}/AdminTreeNode.h
	${ADMIN_PARSER_DIR}/ShutdownTreeNode.cpp
	${ADMIN_PARSER_DIR}/ShutdownTreeNode.h
	CACHE INTERNAL "" )


