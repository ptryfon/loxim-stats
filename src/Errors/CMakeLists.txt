
include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR})

set(Errors_STAT_SRCS
ErrorConsole.cpp
ErrorConsole.h
ErrorMsg.cpp
Errors.h
Exceptions.cpp
Exceptions.h) 

add_library(Errors STATIC ${Errors_STAT_SRCS})
target_link_libraries(Errors Util Config)
