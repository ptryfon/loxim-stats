SET(LOGDIR "../Log")

set(Log_SRCS
	${LOGDIR}/Backup.cpp
	${LOGDIR}/Backup.h
	${LOGDIR}/CrashRecovery.cpp
	${LOGDIR}/CrashRecovery.h
	${LOGDIR}/LogIO.cpp
	${LOGDIR}/LogIO.h
	${LOGDIR}/LogRecord.cpp
	${LOGDIR}/LogRecord.h
	${LOGDIR}/Logs.cpp
	${LOGDIR}/LogsGlobals.cpp
	${LOGDIR}/Logs.h
	${LOGDIR}/LogThread.cpp
	${LOGDIR}/LogThread.h CACHE INTERNAL "" )

