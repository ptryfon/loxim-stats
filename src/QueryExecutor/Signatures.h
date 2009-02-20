#ifndef _SIGNATURES_H_
#define _SIGNATURES_H_

#include <string>
#include <map>
#include <vector>

namespace QParser {class DeclareDefineNode; class Signature; class TreeNode; class SigColl;}
namespace Store {class LogicalID; class ObjectPointer;}
namespace TManager {class Transaction;}
namespace Errors {class ErrorConsole;}

using namespace TManager;
using namespace QParser;
using namespace Store;
using namespace Errors;
using namespace std;

namespace QExecutor
{
	class QueryResult;

	class Signatures
	{
		private:
			ErrorConsole *ec;
	
		public:
			Signatures();
			
			int objDeclRec(DeclareDefineNode *obd, string rootName, bool typeDef, string ownerName, vector<string> *queries, bool topLevel, Transaction *tr);

			int checkUpInRootCardMap(string optrName, map<string, int> &delRootMap, Transaction *tr);
			int checkUpInDelSubMap(LogicalID *ptLid, string name, map<std::pair<unsigned int, string>, int> &delRootMap, Transaction *tr);
			int checkDelCard(QueryResult *arg, QueryResult *&final, Transaction *tr);

			int checkSubCardsRec(Signature *sig, ObjectPointer *optr, Transaction *tr);
			int checkSubCardsRec(Signature *sig, QueryResult *res, bool isBinder, int &obtsSize);
			int checkSingleSubCard(SigColl *sigc, QueryResult *single, map<string, int> &subMap);
			int coerceOperate(int cType, QueryResult *arg, QueryResult *&final, TreeNode *tree, Transaction *tr);
	
	};
}

#endif //_SIGNATURES_H_
