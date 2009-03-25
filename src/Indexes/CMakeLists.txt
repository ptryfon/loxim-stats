
include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${INCDIR})

set(Indexes_STAT_SRCS
BTree.cpp
BTree.h
CachedNode.cpp
CachedNode.h
Cleaner.cpp
Cleaner.h
Comparator.cpp
Comparator.h
const.cpp
const.h
Constraints.cpp
Constraints.h
EntrySieve.cpp
EntrySieve.h
IndexHandler.cpp
IndexHandler.h
IndexManager.cpp
IndexManager.h
LeafTraveller.cpp
LeafTraveller.h
Metadata.cpp
Monitor.cpp
Monitor.h
Node.cpp
Node.h
NonloggedDataSerializer.cpp
NonloggedDataSerializer.h
OptimizedField.cpp
OptimizedField.h
QueryOptimizer.cpp
QueryOptimizer.h
RootEntry.cpp
RootEntry.h
ToStringBuilder.cpp
ToStringBuilder.h
TransactionImpact.cpp
TransactionImpact.h
VisibilityResolver.cpp
VisibilityResolver.h)

add_library(Indexes STATIC ${Indexes_STAT_SRCS})
