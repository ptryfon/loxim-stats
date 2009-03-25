
include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR})

set(Config_STAT_SRCS
SBQLConfig.cpp
SBQLConfig.h)

add_library(Config STATIC ${Config_STAT_SRCS})
target_link_libraries(Config SystemStats)
