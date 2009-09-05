set(UTILDIR "../Util")

set(Util_SRCS
	${UTILDIR}/Backtrace.cpp
	${UTILDIR}/Backtrace.h
	${UTILDIR}/Concurrency.cpp
	${UTILDIR}/Concurrency.h
	${UTILDIR}/InfLoopThread.cpp
	${UTILDIR}/InfLoopThread.h
	${UTILDIR}/SignalReceiver.cpp
	${UTILDIR}/SignalReceiver.h
	${UTILDIR}/SignalRouter.cpp
	${UTILDIR}/SignalRouter.h
	${UTILDIR}/Thread.h
	${UTILDIR}/Thread.cpp
	${UTILDIR}/smartptr.h
	${UTILDIR}/Misc.h CACHE INTERNAL "" )


#add_library(Util STATIC ${Util_STAT_SRCS})
#set_target_properties(Util PROPERTIES OUTPUT_NAME util)
#set_target_properties(Util PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#target_link_libraries(Util Errors)

#IF (NOT DO_NOT_BUILD_SHARED)
#	add_library(Util_shared SHARED ${Util_STAT_SRCS})
#	set_target_properties(Util_shared PROPERTIES OUTPUT_NAME util)
#	set_target_properties(Util_shared PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#ENDIF (NOT DO_NOT_BUILD_SHARED)
