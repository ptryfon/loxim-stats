#include "ResultStack.h"
#include "QueryResult.h"
#include "Errors/ErrorConsole.h"

namespace QExecutor
{
	ResultStack::ResultStack() { ec = &ErrorConsole::get_instance(EC_QUERY_EXECUTOR); }
	ResultStack::~ResultStack() { this->deleteAll(); }

	int ResultStack::push(QueryResult *r) 
	{
		rs.push_back(r);
		debug_print(*ec,  "[QE] Result Stack pushed");
		return 0;
	}

	int ResultStack::pop(QueryResult *&r)
	{
		if (rs.empty()) 
		{
			debug_print(*ec,  (ErrQExecutor | EQEmptySet));
			return (ErrQExecutor | EQEmptySet);
		}
		else 
		{
			r=(rs.back());
			rs.pop_back();
		}
		debug_print(*ec,  "[QE] Result Stack popped");
		return 0;
	}

	bool ResultStack::empty() { return rs.empty(); }
	int ResultStack::size() { return rs.size(); }

	void ResultStack::deleteAll() 
	{
		for (unsigned int i = 0; i < (rs.size()); i++) {
			delete rs.at(i);
		};
	}

	string ResultStack::toString() 
	{
		stringstream c;
		string sizeS;
		c << rs.size(); c >> sizeS;
		string result = "[ResultStack] size=" + sizeS + "\n";
		for( unsigned int i = 0; i < rs.size(); i++ ) 
		{
			result += rs[i]->toString( 1, true, "rs_elem" );
		}
		return result;
	}
}
