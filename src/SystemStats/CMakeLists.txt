set(SYSTEMSTATSDIR "../SystemStats")

set(SystemStats_SRCS
	${SYSTEMSTATSDIR}/AllStats.cpp
	${SYSTEMSTATSDIR}/AllStats.h
	${SYSTEMSTATSDIR}/ConfigStats.cpp
	${SYSTEMSTATSDIR}/ConfigStats.h
	${SYSTEMSTATSDIR}/QueriesStats.cpp
	${SYSTEMSTATSDIR}/QueriesStats.h
	${SYSTEMSTATSDIR}/SessionStats.cpp
	${SYSTEMSTATSDIR}/SessionStats.h
	${SYSTEMSTATSDIR}/StoreStats.cpp
	${SYSTEMSTATSDIR}/StoreStats.h
	${SYSTEMSTATSDIR}/SystemStats.cpp
	${SYSTEMSTATSDIR}/SystemStats.h
	${SYSTEMSTATSDIR}/TransactionsStats.cpp
	${SYSTEMSTATSDIR}/TransactionsStats.h CACHE INTERNAL "" )

#add_library(SystemStats STATIC ${SystemStats_STAT_SRCS})
#set_target_properties(SystemStats PROPERTIES OUTPUT_NAME systemstats)
#set_target_properties(SystemStats PROPERTIES CLEAN_DIRECT_OUTPUT 1)

#IF (NOT DO_NOT_BUILD_SHARED)
#	add_library(SystemStats_shared SHARED ${SystemStats_STAT_SRCS})
#	set_target_properties(SystemStats_shared PROPERTIES OUTPUT_NAME systemstats)
#	set_target_properties(SystemStats_shared PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#ENDIF (NOT DO_NOT_BUILD_SHARED)
