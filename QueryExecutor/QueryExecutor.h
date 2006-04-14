#ifndef _QUERYEXECUTOR_H
#define _QUERYEXECUTOR_H

#include <stdio.h>
#include <string>
#include <vector>
   
#include "QueryResult.h"
#include "TransactionManager/Transaction.h"
#include "Store/Store.h"
#include "Store/DBDataValue.h"
#include "Store/DBLogicalID.h"
#include "QueryParser/QueryParser.h"
#include "QueryParser/TreeNode.h"
#include "Errors/Errors.h"
#include "Errors/ErrorConsole.h"

using namespace QParser;
using namespace TManager;
using namespace Errors;
using namespace Store;
using namespace std;

namespace QExecutor
{
	class EnvironmentStack
	{
	protected: 
		ErrorConsole *ec;
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
		ErrorConsole *ec;
		Transaction *tr;
		EnvironmentStack stack;
		int stop;
		bool evalStopped() { return ( stop != 0 ); };
		int executeRecQuery(TreeNode *tree, QueryResult **result);
		int combine(NonAlgOpNode::nonAlgOp op, QueryResult *curr, QueryResult *lRes, QueryResult *&partial);
		int merge(NonAlgOpNode::nonAlgOp op, QueryResult *partial, QueryResult *&final);
		int unOperate(UnOpNode::unOp op, QueryResult *arg, QueryResult *&final);
		int algOperate(AlgOpNode::algOp op, QueryResult *lArg, QueryResult *rArg, QueryResult *&final);
		int derefQuery(QueryResult *arg, QueryResult *&res);
		int refQuery(QueryResult *arg, QueryResult *&res);
		int isIncluded(QueryResult *elem, QueryResult *set, bool &score);
		int sortBag(QueryBagResult *inBag, QueryBagResult *&outBag);
		int objectFromBinder(QueryResult *res, ObjectPointer *&newObject);
	public:
		QueryExecutor() { tr = NULL; ec = new ErrorConsole("QueryExecutor"); stop = 0; };
		~QueryExecutor();
		int executeQuery(TreeNode *tree, QueryResult **result);
		void stopExecuting() { stop = 65535; };
		int abort();
	};
	
}

#endif
