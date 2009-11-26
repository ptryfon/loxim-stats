set(UTILDIR "../Util")

set(Util_SRCS
	${UTILDIR}/Types.h
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

