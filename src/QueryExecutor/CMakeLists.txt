
include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR})

set(QueryExecutor_STAT_SRCS
AccessMap.cpp
AccessMap.h
BindMap.cpp
BindMap.h
BindNames.h
ClassGraph.cpp
ClassGraph.h
CommonOperations.cpp
CommonOperations.h
EnvironmentStack.cpp
EnvironmentStack.h
ExecutorViews.cpp
ExecutorViews.h
HashMacro.h
InterfaceKey.h
InterfaceMaps.cpp
InterfaceMaps.h
InterfaceMatcher.cpp
InterfaceMatcher.h
InterfaceQuery.cpp
InterfaceQuery.h
OuterSchema.cpp
OuterSchema.h
QueryBuilder.cpp
QueryBuilder.h
QueryExecutor.cpp
QueryExecutor.h
QueryResult.cpp
QueryResult.h
ResultStack.cpp
ResultStack.h
Signatures.cpp
Signatures.h) 

add_library(QueryExecutor STATIC ${QueryExecutor_STAT_SRCS})
