/* Query Executor
   10.2005 - 01.2006

   Authors:
   Beata Golichowska
   Dariusz Gryglas
   Adam Michalik
   Maja Perycz */
   
#include <stdio.h>

#include "QueryResult.h"
#include "Store/Store.h"

namespace QExecutor {

	QueryResult::QueryResult() {};
	QueryResult::~QueryResult() {};

	int QueryResult::addNext(LogicalID *lid) {
		res.push_front(lid);
		return 0;
	}
	
	int QueryResult::getNext(LogicalID *lid) {
		if (res.begin() != NULL) {
			lid = *(res.begin());
			res.pop_front();
			return 0;
		}
		return -1;
	};
}