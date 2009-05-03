
include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR})

set(CBindings_SRCS
	access.c
	convert.cpp
	exports.cpp
	loxim_client.h)




IF (NOT DO_NOT_BUILD_SHARED)
	add_library(loxim_client_shared SHARED ${CBindings_SRCS})
	target_link_libraries(loxim_client_shared Client readline pthread Protocol ProtocolStreams)
	#target_link_libraries(loxim_client_shared Client_shared readline pthread Protocol_shared ProtocolStreams_shared)
	set_target_properties(loxim_client_shared PROPERTIES OUTPUT_NAME loxim_client)
	set_target_properties(loxim_client_shared PROPERTIES CLEAN_DIRECT_OUTPUT 1)
	#	target_link_libraries(loxim_client_shared LINK_INTERFACE_LIBRARIES Client_shared READLINE_LIBRARY pthread)
	install(TARGETS loxim_client_shared DESTINATION lib) 
ENDIF (NOT DO_NOT_BUILD_SHARED)

add_library(loxim_client_static STATIC ${CBindings_SRCS})
set_target_properties(loxim_client_static PROPERTIES OUTPUT_NAME loxim_client)
set_target_properties(loxim_client_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

install(TARGETS loxim_client_static DESTINATION lib)
install(FILES loxim_client.h DESTINATION include)
