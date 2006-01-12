#ifndef _QUERYEXECUTOR_H
#define _QUERYEXECUTOR_H

#include <string>
#include <vector>

using namespace QParser;
using namespace TManager;
using namespace std;

namespace QExecutor
{
	class EnvironmentStack
	{
	protected: 
		vector<QueryBagResult*> es;
	public:
		EnvironmentStack();
		~EnvironmentStack();
		int push(QueryBagResult *r);
		int pop();
		int top(QueryBagResult *&r);
		bool empty();
		int size();
		int bindName(string name, QueryResult *&r);
		void deleteAll();
	};

	class QueryExecutor
	{
	protected:
		Transaction *tr;
		EnvironmentStack stack;
	public:
		QueryExecutor() { tr = NULL; };
		~QueryExecutor();
		int executeQuery(TreeNode *tree, QueryResult **result);
	};
	
}

#endif
