#ifndef _ENVIRONMENTSTACK_H
#define _ENVIRONMENTSTACK_H

#include <string>
#include <vector>
 
#include "HashMacro.h"
#include "QueryParser/TreeNode.h"

namespace QExecutor { class QueryResult; class QueryBagResult;}
namespace TManager {class Transaction;}
namespace Store {class LogicalID; class DataValue;}
namespace Errors {class ErrorConsole;}
namespace QParser {class TreeNode;}

using namespace QParser;
using namespace TManager;
using namespace Errors;
using namespace Store;
using namespace std;

namespace QExecutor
{
	typedef hash_map<unsigned int, SetOfLids*> SectionToClassMap;
	typedef map<unsigned int, std::pair<string, LogicalID*> > SectionToInterfaceMap;
	
	class EnvironmentStack
	{
	protected: 
		//lidy zawarte w tej kolekcji naleza do grafu klas 
		//i nie nalezy ich niszczyc w tej klasie
		SectionToClassMap classesPerSection;
		SectionToInterfaceMap interfacePerSection;
		ErrorConsole *ec;
		vector<QueryBagResult*> es;
		unsigned int sectionDBnumber;

	public:
		vector<int> es_priors;
		int actual_prior;
		//za niszczenie tego obiektu odpowiada ClassGraph
		LogicalID* actualBindClassLid;

		EnvironmentStack();
		virtual ~EnvironmentStack();
		int push(QueryBagResult *r, Transaction *&tr, QueryExecutor *qe);
		int pushClasses(DataValue* dv, QueryExecutor *qe, bool& classFound);
		int pushInterface(string interfaceName, LogicalID* boundClassLid);
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
        string toString();
	};	
}

#endif
