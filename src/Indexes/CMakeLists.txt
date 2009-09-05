set(INDEXESDIR "../Indexes") 

set(Indexes_SRCS
	${INDEXESDIR}/BTree.cpp
	${INDEXESDIR}/BTree.h
	${INDEXESDIR}/CachedNode.cpp
	${INDEXESDIR}/CachedNode.h
	${INDEXESDIR}/Cleaner.cpp
	${INDEXESDIR}/Cleaner.h
	${INDEXESDIR}/Comparator.cpp
	${INDEXESDIR}/Comparator.h
	${INDEXESDIR}/const.cpp
	${INDEXESDIR}/const.h
	${INDEXESDIR}/Constraints.cpp
	${INDEXESDIR}/Constraints.h
	${INDEXESDIR}/EntrySieve.cpp
	${INDEXESDIR}/EntrySieve.h
	${INDEXESDIR}/IndexHandler.cpp
	${INDEXESDIR}/IndexHandler.h
	${INDEXESDIR}/IndexManager.cpp
	${INDEXESDIR}/IndexManager.h
	${INDEXESDIR}/LeafTraveller.cpp
	${INDEXESDIR}/LeafTraveller.h
	${INDEXESDIR}/Metadata.cpp
	${INDEXESDIR}/Monitor.cpp
	${INDEXESDIR}/Monitor.h
	${INDEXESDIR}/Node.cpp
	${INDEXESDIR}/Node.h
	${INDEXESDIR}/NonloggedDataSerializer.cpp
	${INDEXESDIR}/NonloggedDataSerializer.h
	${INDEXESDIR}/OptimizedField.cpp
	${INDEXESDIR}/OptimizedField.h
	${INDEXESDIR}/QueryOptimizer.cpp
	${INDEXESDIR}/QueryOptimizer.h
	${INDEXESDIR}/RootEntry.cpp
	${INDEXESDIR}/RootEntry.h
	${INDEXESDIR}/ToStringBuilder.cpp
	${INDEXESDIR}/ToStringBuilder.h
	${INDEXESDIR}/TransactionImpact.cpp
	${INDEXESDIR}/TransactionImpact.h
	${INDEXESDIR}/VisibilityResolver.cpp
	${INDEXESDIR}/VisibilityResolver.h CACHE INTERNAL "" )

