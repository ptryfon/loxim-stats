
include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR})

set(Log_STAT_SRCS
Backup.cpp
Backup.h
CrashRecovery.cpp
CrashRecovery.h
LogIO.cpp
LogIO.h
LogRecord.cpp
LogRecord.h
Logs.cpp
LogsGlobals.cpp
Logs.h
LogThread.cpp
LogThread.h)

add_library(Log STATIC ${Log_STAT_SRCS})
