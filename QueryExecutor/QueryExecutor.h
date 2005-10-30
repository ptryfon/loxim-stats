#ifndef _QUERYEXECUTOR_H
#define _QUERYEXECUTOR_H

namespace QExecutor
{
	class QueryExecutor
	{
	public:
		QueryExecutor() {};
		~QueryExecutor();
		int queryResult(QueryTree *tree, QueryResult *result);
		int init(TransactionManager *t);
	};
}

#endif
