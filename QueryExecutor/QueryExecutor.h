#ifndef _QUERYEXECUTOR_H
#define _QUERYEXECUTOR_H

#include <stdio.h>
#include <string>
#include <vector>
   
#include "QueryResult.h"
#include "EnvironmentStack.h"
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
	
	class ResultStack
	{
	protected: 
		ErrorConsole *ec;
		vector<QueryResult*> rs;
	public:
		ResultStack();
		~ResultStack();
		int push(QueryResult *r);
		int pop(QueryResult *&r);
		bool empty();
		int size();
		void deleteAll();
	};

	class QueryExecutor
	{
	protected:
		ErrorConsole *ec;
		Transaction *tr;
		EnvironmentStack *envs;
		ResultStack *qres;
		int stop;
		bool evalStopped() { return ( stop != 0 ); };
		int executeRecQuery(TreeNode *tree);
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
		QueryExecutor() { envs = new EnvironmentStack(); qres = new ResultStack(); tr = NULL; ec = new ErrorConsole("QueryExecutor"); stop = 0; };
		~QueryExecutor();
		int executeQuery(TreeNode *tree, QueryResult **result);
		void stopExecuting() { stop = 65535; };
		int abort();
	};
	
}

#endif
