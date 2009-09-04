
include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR})

if (G_PERF_TOOLS_TESTS_ENABLED)
	set(EXTRALIBS ${TCMALLOC_LIBRARY})
endif (G_PERF_TOOLS_TESTS_ENABLED)

add_executable(lsbql lsbql.cpp)
add_dependencies(lsbql lsbqlLib)
target_link_libraries(lsbql lsbqlLib)


add_executable(loxim_server loxim_server.cpp)
add_dependencies(loxim_server serverLib)
target_link_libraries(loxim_server serverLib ${EXTRALIBS})

add_library(lsbqlLib STATIC lsbql.cpp)
target_link_libraries(lsbqlLib Errors Client Util ProtocolAllInOne Config ${READLINE_LIBRARY} pthread)

add_library(serverLib STATIC loxim_server.cpp)
target_link_libraries(serverLib Log Lock Config Errors Store ARIESLog Indexes QueryParser TransactionManager SystemStats QueryExecutor AdminParser TypeCheck Server Util ProtocolAllInOne pthread)

install(TARGETS lsbql loxim_server DESTINATION bin)
