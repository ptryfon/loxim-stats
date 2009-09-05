set(CONFIGDIR "../Config")

set(Config_SRCS
	${CONFIGDIR}/SBQLConfig.cpp
	${CONFIGDIR}/SBQLConfig.h CACHE INTERNAL "" )

#add_library(Config STATIC ${Config_STAT_SRCS})
#target_link_libraries(Config SystemStats)
#set_target_properties(Config PROPERTIES OUTPUT_NAME config)
#set_target_properties(Config PROPERTIES CLEAN_DIRECT_OUTPUT 1)

#IF (NOT DO_NOT_BUILD_SHARED)
#	add_library(Config_shared SHARED ${Config_STAT_SRCS})
#	target_link_libraries(Config_shared SystemStats_shared)
	#	set_target_properties(Config_shared PROPERTIES OUTPUT_NAME config)
	#	set_target_properties(Config_shared PROPERTIES CLEAN_DIRECT_OUTPUT 1)
	#ENDIF (NOT DO_NOT_BUILD_SHARED)
