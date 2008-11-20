#ifndef _INTERFACE_QUERY_H_
#define _INTERFACE_QUERY_H_
#include <QueryExecutor/InterfaceMatcher.h>

namespace QParser
{
    class QueryNode;
}

namespace Errors
{
    class ErrorConsole;
}

namespace QExecutor
{

    class InterfaceQuery
    {
	public:
	    static int produceQuery(Schemas::Schema *interfaceSchema, Errors::ErrorConsole *ec, const string& classInvariantName, QParser::QueryNode *& query);
	private:
	    static int constructSelection(Schemas::Schema *interfaceSchema, Errors::ErrorConsole *ec, QParser::QueryNode *& selectionQuery);
    };
}

#endif //_INTERFACE_QUERY_H
