#ifndef _ENVIRONMENTSTACK_H
#define _ENVIRONMENTSTACK_H

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
		int bindName(string name, int sectionNo, QueryResult *&r);
		void deleteAll();
	};
	
}

#endif
