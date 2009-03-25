
include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR})

set(Server_STAT_SRCS
Clipboard.cpp
Clipboard.h
Server.cpp
Server.h
Session.cpp
Session.h)

add_library(Server STATIC ${Server_STAT_SRCS})
target_link_libraries(Server Log Lock Config Errors Store ARIESLog Indexes QueryParser TransactionManager SystemStats QueryExecutor AdminParser TypeCheck Server Util ProtocolEnums Protocol ProtocolStreams ProtocolPackagesData ProtocolPackages)
