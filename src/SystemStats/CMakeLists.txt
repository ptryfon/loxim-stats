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

