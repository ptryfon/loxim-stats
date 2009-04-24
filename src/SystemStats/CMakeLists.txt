
include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR})

set(SystemStats_STAT_SRCS
	AllStats.cpp
	AllStats.h
	ConfigStats.cpp
	ConfigStats.h
	QueriesStats.cpp
	QueriesStats.h
	SessionStats.cpp
	SessionStats.h
	StoreStats.cpp
	StoreStats.h
	SystemStats.cpp
	SystemStats.h
	TransactionsStats.cpp
	TransactionsStats.h)

add_library(SystemStats STATIC ${SystemStats_STAT_SRCS})
set_target_properties(SystemStats PROPERTIES OUTPUT_NAME systemstats)
set_target_properties(SystemStats PROPERTIES CLEAN_DIRECT_OUTPUT 1)

IF (NOT DO_NOT_BUILD_SHARED)
	add_library(SystemStats_shared SHARED ${SystemStats_STAT_SRCS})
	set_target_properties(SystemStats_shared PROPERTIES OUTPUT_NAME systemstats)
	set_target_properties(SystemStats_shared PROPERTIES CLEAN_DIRECT_OUTPUT 1)
ENDIF (NOT DO_NOT_BUILD_SHARED)
