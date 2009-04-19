
include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR})

set(CClient_STAT_SRCS
access.c
convert.cpp
exports.cpp
liblx.h)

add_library(CClient STATIC ${CClient_STAT_SRCS})

IF (NOT DO_NOT_BUILD_SHARED)
  add_library(CBindings SHARED ${CClient_STAT_SRCS})
  install(TARGETS CBindings DESTINATION lib) 
ENDIF (NOT DO_NOT_BUILD_SHARED)
