
include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}/src ${INCDIR})


set(Util_STAT_SRCS
	Backtrace.cpp
	Backtrace.h
	Concurrency.cpp
	Concurrency.h
	InfLoopThread.cpp
	InfLoopThread.h
	SignalReceiver.cpp
	SignalReceiver.h
	SignalRouter.cpp
	SignalRouter.h
	smartptr.h)


add_library(Util STATIC ${Util_STAT_SRCS})
set_target_properties(Util PROPERTIES OUTPUT_NAME util)
set_target_properties(Util PROPERTIES CLEAN_DIRECT_OUTPUT 1)

IF (NOT DO_NOT_BUILD_SHARED)
	add_library(Util_shared SHARED ${Util_STAT_SRCS})
	set_target_properties(Util_shared PROPERTIES OUTPUT_NAME util)
	set_target_properties(Util_shared PROPERTIES CLEAN_DIRECT_OUTPUT 1)
ENDIF (NOT DO_NOT_BUILD_SHARED)
