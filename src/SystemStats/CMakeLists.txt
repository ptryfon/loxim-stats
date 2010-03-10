set(SYSTEMSTATSDIR "../SystemStats")

set(SystemStats_SRCS
	${SYSTEMSTATSDIR}/Statistics.cpp
	${SYSTEMSTATSDIR}/Statistics.h
	${SYSTEMSTATSDIR}/DiskUsageStats.cpp
	${SYSTEMSTATSDIR}/DiskUsageStats.h
	${SYSTEMSTATSDIR}/ShareStat.h
	${SYSTEMSTATSDIR}/ConfigStats.cpp
	${SYSTEMSTATSDIR}/ConfigStats.h
	${SYSTEMSTATSDIR}/QueriesStats.cpp
	${SYSTEMSTATSDIR}/QueriesStats.h
	${SYSTEMSTATSDIR}/SessionStats.cpp
	${SYSTEMSTATSDIR}/SessionStats.h
	${SYSTEMSTATSDIR}/StoreStats.cpp
	${SYSTEMSTATSDIR}/StoreStats.h
	${SYSTEMSTATSDIR}/TransactionsStats.cpp
	${SYSTEMSTATSDIR}/TransactionsStats.h CACHE INTERNAL "" )

#IF (QT4_FOUND)
#   add_subdirectory(StatsDisplay)
#ENDIF (QT4_FOUND)


