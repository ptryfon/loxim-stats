set(ERRORSDIR "../Errors")

set(Errors_SRCS
	${ERRORSDIR}/ErrorConsole.cpp
	${ERRORSDIR}/ErrorConsole.h
	${ERRORSDIR}/ErrorMsg.cpp
	${ERRORSDIR}/Errors.h
	${ERRORSDIR}/Exceptions.cpp
	${ERRORSDIR}/Exceptions.h CACHE INTERNAL "" ) 

#add_library(Errors STATIC ${Errors_STAT_SRCS})
#target_link_libraries(Errors Util Config)
#set_target_properties(Errors PROPERTIES OUTPUT_NAME errors)
#set_target_properties(Errors PROPERTIES CLEAN_DIRECT_OUTPUT 1)

#IF (NOT DO_NOT_BUILD_SHARED)
	#add_library(Errors_shared SHARED ${Errors_STAT_SRCS})
	#target_link_libraries(Errors_shared Util_shared Config_shared)
	#set_target_properties(Errors_shared PROPERTIES OUTPUT_NAME errors)
	#set_target_properties(Errors_shared PROPERTIES CLEAN_DIRECT_OUTPUT 1)
	#ENDIF (NOT DO_NOT_BUILD_SHARED)
