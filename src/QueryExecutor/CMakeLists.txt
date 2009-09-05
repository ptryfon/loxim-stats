set(QUERYEXECUTORDIR "../QueryExecutor")

set(QueryExecutor_SRCS
	${QUERYEXECUTORDIR}/AccessMap.cpp
	${QUERYEXECUTORDIR}/AccessMap.h
	${QUERYEXECUTORDIR}/BindMap.cpp
	${QUERYEXECUTORDIR}/BindMap.h
	${QUERYEXECUTORDIR}/BindNames.h
	${QUERYEXECUTORDIR}/ClassGraph.cpp
	${QUERYEXECUTORDIR}/ClassGraph.h
	${QUERYEXECUTORDIR}/CommonOperations.cpp
	${QUERYEXECUTORDIR}/CommonOperations.h
	${QUERYEXECUTORDIR}/EnvironmentStack.cpp
	${QUERYEXECUTORDIR}/EnvironmentStack.h
	${QUERYEXECUTORDIR}/ExecutorViews.cpp
	${QUERYEXECUTORDIR}/ExecutorViews.h
	${QUERYEXECUTORDIR}/HashMacro.h
	${QUERYEXECUTORDIR}/InterfaceKey.h
	${QUERYEXECUTORDIR}/InterfaceMaps.cpp
	${QUERYEXECUTORDIR}/InterfaceMaps.h
	${QUERYEXECUTORDIR}/InterfaceMatcher.cpp
	${QUERYEXECUTORDIR}/InterfaceMatcher.h
	${QUERYEXECUTORDIR}/InterfaceQuery.cpp
	${QUERYEXECUTORDIR}/InterfaceQuery.h
	${QUERYEXECUTORDIR}/OuterSchema.cpp
	${QUERYEXECUTORDIR}/OuterSchema.h
	${QUERYEXECUTORDIR}/QueryBuilder.cpp
	${QUERYEXECUTORDIR}/QueryBuilder.h
	${QUERYEXECUTORDIR}/QueryExecutor.cpp
	${QUERYEXECUTORDIR}/QueryExecutor.h
	${QUERYEXECUTORDIR}/QueryResult.cpp
	${QUERYEXECUTORDIR}/QueryResult.h
	${QUERYEXECUTORDIR}/ResultStack.cpp
	${QUERYEXECUTORDIR}/ResultStack.h
	${QUERYEXECUTORDIR}/Signatures.cpp
	${QUERYEXECUTORDIR}/Signatures.h CACHE INTERNAL "" ) 

