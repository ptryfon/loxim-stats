#ifndef _ENVIRONMENTSTACK_H
#define _ENVIRONMENTSTACK_H

#include <stdio.h>
#include <string>
#include <vector>
  
#include "ClassGraph.h"
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
	typedef hash_map<unsigned int, SetOfLids*> SectionToClassMap;
	
	class EnvironmentStack
	{
	protected: 
		//lidy zawarte w tej kolekcji naleza do grafu klas
		//i nie nalezy ich niszczyc w tej klasie
		SectionToClassMap classesPerSection;
		ErrorConsole *ec;
		vector<QueryBagResult*> es;
		unsigned int sectionDBnumber;
	public:
		EnvironmentStack();
		virtual ~EnvironmentStack();
		int push(QueryBagResult *r, Transaction *&tr, QueryExecutor *qe);
		int pushClasses(DataValue* dv, QueryExecutor *qe, bool& classFound);
		int pop();
		int pushDBsection();
		int popDBsection();
		int top(QueryBagResult *&r);
		bool empty();
		int size();
		int bindName(string name, int sectionNo, Transaction *&tr, QueryExecutor *qe, QueryResult *&r, string iName ="");
		int bindProcedureName(string name, unsigned int queries_size, Transaction *&tr, QueryExecutor *qe, string &code, vector<string> &params, int &bindSectionNo, LogicalID*& bindClassLid);
		unsigned int getSectionDBnumber();
		void deleteAll();
		
        	string toString() {
			stringstream c;
			//string sectionDBnumberS;
			c << "[EnvironmentStack] sectionDBnumber=" << sectionDBnumber 
				<< " actual_prior=" << actual_prior << endl;
			string result = c.str();
			for( unsigned int i = 0; i < es.size(); i++ ) {
				stringstream s;
				s << "es_elem prior: " << es_priors.at(i);
				result += es[i]->toString( 1, true, s.str() );
			}
			return result;
		}
		
		vector<int> es_priors;
		int actual_prior;
		//za niszczenie tego obiektu odpowiada ClassGraph
		LogicalID* actualBindClassLid;
	};
	
}

#endif
