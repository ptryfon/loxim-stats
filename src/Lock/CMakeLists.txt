
include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR})

set(Lock_STAT_SRCS
Comparator.cpp
Comparator.h
Lock.cpp
Lock.h
SingleLock.cpp
SingleLock.h)

add_library(Lock STATIC ${Lock_STAT_SRCS})
