
include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR})

set(ARIESLog_STAT_SRCS
LogRecord.cpp
LogRecord.h
Logs.cpp
Logs.h)

add_library(ARIESLog STATIC ${ARIESLog_STAT_SRCS})
