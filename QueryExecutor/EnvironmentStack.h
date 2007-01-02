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
		unsigned int sectionDBnumber;
		int procCheck(unsigned int qSize, LogicalID *lid, Transaction *&tr, QueryExecutor *qe, string &code, vector<string> &prms, int &f);
	public:
		EnvironmentStack();
		virtual ~EnvironmentStack();
		int push(QueryBagResult *r);
		int pop();
		int pushDBsection();
		int popDBsection();
		int top(QueryBagResult *&r);
		bool empty();
		int size();
		int bindName(string name, int sectionNo, Transaction *&tr, QueryExecutor *qe, QueryResult *&r);
		int bindProcedureName(string name, unsigned int queries_size, Transaction *&tr, QueryExecutor *qe, string &code, vector<string> &params);
		void deleteAll();
        	string toString() {
        	
          stringstream c;
          string sectionDBnumberS;
          c << sectionDBnumber; c >> sectionDBnumberS;
          string result = "[EnvironmentStack] sectionDBnumber=" + sectionDBnumberS + "\n";

          for( unsigned int i = 0; i < es.size(); i++ ) {
            result += es[i]->toString( 1, true, "es_elem" );
          }

          return result;
        }
	};
	
}

#endif
