#ifndef _COMMON_OPERATIONS_H_
#define _COMMON_OPERATIONS_H_

#include <string>
#include "QueryParser/TreeNode.h"

namespace TManager {class Transaction;}
namespace Errors {class ErrorConsole;}
namespace QExecutor {class QueryResult;}

using namespace QExecutor;
using namespace QParser;
using namespace Errors;
using namespace TManager;

namespace CommonOperations
{
	int nameTaken(string name, Transaction *tr, bool &taken);
	int isIncluded(QueryResult *elem, QueryResult *set, bool &score);
	int merge(NonAlgOpNode::nonAlgOp op, QueryResult *partial, ErrorConsole *ec, QueryResult *&final);
}



#endif //_COMMON_OPERATIONS_H_
