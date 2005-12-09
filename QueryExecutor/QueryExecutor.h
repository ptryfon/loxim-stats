#ifndef _QUERYEXECUTOR_H
#define _QUERYEXECUTOR_H

using namespace QParser;
using namespace TManager;

namespace QExecutor
{
	class QueryExecutor
	{
	protected:
		Transaction *tr;
	public:
		QueryExecutor() { tr = NULL; };
		~QueryExecutor();
		int executeQuery(TreeNode *tree, QueryResult **result);
	};
}

#endif
