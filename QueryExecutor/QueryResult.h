#ifndef _QUERYRESULT_H_
#define _QUERYRESULT_H_

#include <list>

#include "Store/Store.h"

using std::list;

namespace QExecutor {

	class QueryResult
	{
		private:
			list <LogicalID *> res;
			int addNext(LogicalID *lid); //adds next LogicalID* to result
		public:    
			QueryResult();
			~QueryResult();
			int getNext(LogicalID *lid); //returns next result (LogicalID*)
	};

}

#endif

