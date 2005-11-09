/* Query Executor
   10.2005 - 01.2006

   Authors:
   Beata Golichowska
   Dariusz Gryglas
   Adam Michalik
   Maja Perycz */
   
#include <stdio.h>
   
#include "QueryResult.h"   
#include "TransactionManager/Transaction.h"
#include "Store/Store.h"
#include "QueryParser/QueryParser.h"
#include "QueryExecutor.h"

namespace QExecutor {

int QueryExecutor::queryResult(QueryTree *tree, QueryResult *result) {
	Transaction *tr;
	LogicalID *lid;
	AccessMode mode;
	ObjectPointer *optr;
	string name;
	DataValue* value;
  
	fprintf(stderr, "QueryExecutor method: queryResult\n");
	fprintf(stderr, "QueryExecutor asking TransactionManager for a new transaction\n");
	if (TransactionManager::getHandle()->createTransaction(tr) != 0) {
		fprintf(stderr, "Error in createTransaction\n");
	}
	fprintf(stderr, "QueryExecutor asking Store for proxy object to calculate query\n");
	//UWAGA! PONIZSZE INSTRUKCJE SA TU TYLKO TYMCZASOWO - TRZEBA JE WKLEIC NA SWITCHA SKLADNIOWEGO...
	//---- tego jeszcze nie jestem pewien - nie do konca zrozumialem TMa
	if (tr->getObjectPointer(lid, mode, optr) != 0) {
		fprintf(stderr, "Error in getObjectPointer\n");
	}
	//---- odpowiednik wywolania CREATE name(value);
	//---- optr jest *wynikiem* tej metody!
	if (tr->createObject(name, value, optr) != 0) {
		fprintf(stderr, "Error in createObject\n");
	}
	//---- odpowiednik wywolania CREATE name; (z pusta wartoscia)
	//---- optr jest *wynikiem* tej metody!
	if (tr->createObject(name, NULL, optr) != 0) {
		fprintf(stderr, "Error in createObject\n");
	}
	//KONIEC
	// !!!!! optr - tu jest wynik naszego zapytania (musimy zwrocic jego LogicalID* - funkcja optr.getLogicalID() )
	
	//tutaj ladnie ukladamy QueryResult
	result = new QueryResult;
	return 0;
}

QueryExecutor::~QueryExecutor() {}
}
