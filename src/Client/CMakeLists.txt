set (CLIENTDIR "../Client")

set(Client_SRCS
	${CLIENTDIR}/Authenticator.cpp
	${CLIENTDIR}/Authenticator.h
	${CLIENTDIR}/ClientConsole.cpp
	${CLIENTDIR}/ClientConsole.h
	${CLIENTDIR}/ConsoleAuthenticator.cpp
	${CLIENTDIR}/ConsoleAuthenticator.h
	${CLIENTDIR}/Client.cpp
	${CLIENTDIR}/Client.h
	${CLIENTDIR}/SimpleClient.cpp
	${CLIENTDIR}/SimpleClient.h CACHE INTERNAL "")

#add_library(Client STATIC ${Client_STAT_SRCS})
#target_link_libraries(Client Errors ProtocolAllInOne)
#set_target_properties(Client PROPERTIES OUTPUT_NAME client)
#set_target_properties(Client PROPERTIES CLEAN_DIRECT_OUTPUT 1)


#IF (NOT DO_NOT_BUILD_SHARED)
#	add_library(Client_shared SHARED ${Client_STAT_SRCS})
#target_link_libraries(Client_shared Errors_shared)
	#set_target_properties(Client_shared PROPERTIES OUTPUT_NAME client)
	#set_target_properties(Client_shared PROPERTIES CLEAN_DIRECT_OUTPUT 1)
	#ENDIF (NOT DO_NOT_BUILD_SHARED)
