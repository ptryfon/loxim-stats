#ifndef _QUERYEXECUTOR_H
#define _QUERYEXECUTOR_H

using namespace QParser;
using namespace TManager;

namespace QExecutor
{
	class QueryExecutor
	{
	public:
		QueryExecutor() {};
		~QueryExecutor();
		int executeQuery(TreeNode *tree, QueryResult **result);
	};
}

#endif
