#ifndef _QUERYEXECUTOR_H
#define _QUERYEXECUTOR_H

namespace QueryExecutor
{
	class QueryExecutor
	{
	public:
		QueryExecutor() {};
		virtual ~QueryExecutor() = 0;
		virtual int queryResult(QueryTree *tree, QueryResult *result) = 0;
		virtual int init(TransactionManager *t) = 0;
	};
}

#endif
