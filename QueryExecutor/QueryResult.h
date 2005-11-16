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

		public:    
			QueryResult();
			~QueryResult();
			int addNext(LogicalID *lid); //adds next LogicalID* to result
			int getNext(LogicalID *lid); //returns in lid next result (LogicalID*); -1 when there is no next result
	};

}

#endif

