include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/Errors ${INCDIR})

if (G_PERF_TOOLS_TESTS_ENABLED)
	set(EXTRALIBS ${TCMALLOC_LIBRARY})
endif (G_PERF_TOOLS_TESTS_ENABLED)

add_executable(lsbql lsbql.cpp)
target_link_libraries(lsbql lsbqlLib)

add_executable(loxim_server loxim_server.cpp)
target_link_libraries(loxim_server serverLib ${EXTRALIBS})


set(ClientExternal_SRCS  ${Errors_SRCS} ${Client_SRCS} ${ProtocolAllInOne_SRCS} ${Util_SRCS})

add_library(lsbqlLib STATIC lsbql.cpp ${ClientExternal_SRCS})
target_link_libraries(lsbqlLib ${READLINE_LIBRARY} pthread)

set(LoximServerExternal_SRCS ${Log_SRCS} 
	${Lock_SRCS} ${Config_SRCS} 
	${Errors_SRCS} ${Store_SRCS} 
	${ARIESLog_SRCS} 
	${Indexes_SRCS} 
	${QueryParser_SRCS} 
	${TransactionManager_SRCS} 
	${SystemStats_SRCS} 
	${QueryExecutor_SRCS} 
	${AdminParser_SRCS} 
	${TypeCheck_SRCS} 
	${Server_SRCS} 
	${Util_SRCS} 
	${ProtocolAllInOne_SRCS}
	${CURRENT_BINARY_DIR}../AdminParser/scanner.cc
	${CURRENT_BINARY_DIR}../QueryParser/scanner.cc
	)

add_library(serverLib STATIC loxim_server.cpp ${LoximServerExternal_SRCS})
SET_SOURCE_FILES_PROPERTIES(${CURRENT_BINARY_DIR}../AdminParser/scanner.cc ${CURRENT_BINARY_DIR}../QueryParser/scanner.cc PROPERTIES GENERATED 1)
add_DEPENDENCIES(serverLib ala generate_admin_parser generate_admin_lexer generate_query_parser generate_query_lexer)

target_link_libraries(serverLib pthread)

IF (NOT DO_NOT_BUILD_SHARED)
	add_library(loxim_client_shared SHARED ${CBindings_SRCS})
	target_link_libraries(loxim_client_shared lsbqlLib)
	set_target_properties(loxim_client_shared PROPERTIES OUTPUT_NAME loxim_client)
	set_target_properties(loxim_client_shared PROPERTIES CLEAN_DIRECT_OUTPUT 1)
	install(TARGETS loxim_client_shared DESTINATION lib) 
ENDIF (NOT DO_NOT_BUILD_SHARED)

add_library(loxim_client_static STATIC ${CBindings_SRCS})
set_target_properties(loxim_client_static PROPERTIES OUTPUT_NAME loxim_client)
set_target_properties(loxim_client_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

install(TARGETS loxim_client_static DESTINATION lib)
install(FILES loxim_client.h DESTINATION include)
