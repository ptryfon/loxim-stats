
include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR})


set(TransactionManager_STAT_SRCS
Mutex.cpp
Mutex.h
RWSemaphore.cpp
RWSemaphore.h
RWUJSemaphore.cpp
RWUJSemaphore.h
Semaphore.cpp
Semaphore.h
SemHeaders.h
Transaction.cpp
Transaction.h)

add_library(TransactionManager STATIC ${TransactionManager_STAT_SRCS})
target_link_libraries(TransactionManager Errors)
