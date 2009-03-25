
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
smartptr.h
thread_test.cpp)


add_library(Util STATIC ${Util_STAT_SRCS})
