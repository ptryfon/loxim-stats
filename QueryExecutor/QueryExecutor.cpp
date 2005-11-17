/* Query Executor
   10.2005 - 01.2006

   Authors:
   Beata Golichowska
   Dariusz Gryglas
   Adam Michalik
   Maja Perycz */
   
#include <stdio.h>
#include <vector>
   
#include "QueryResult.h"   
#include "TransactionManager/Transaction.h"
#include "Store/Store.h"
#include "QueryParser/QueryParser.h"
#include "QueryParser/TreeNode.h"
#include "QueryExecutor.h"

using namespace QParser;
using namespace TManager;

namespace QExecutor {

int QueryExecutor::queryResult(TreeNode *tree, QueryResult *result) {
	Transaction *tr;
	LogicalID *lid;
	//AccessMode mode;
	ObjectPointer *optr;
	string name;
	DataValue* value;
	string treeNodeType; //to tylko tymczasowo.
	vector<ObjectPointer*>* vec;
	int vecSize;
	  
	fprintf(stderr, "QueryExecutor method: queryResult\n");
	fprintf(stderr, "QueryExecutor asking TransactionManager for a new transaction\n");
	
	if (TransactionManager::getHandle()->createTransaction(tr) != 0) {
		fprintf(stderr, "Error in createTransaction\n");
	}
	//fprintf(stderr, "QueryExecutor asking Store for proxy object to calculate query\n");
	//UWAGA! PONIZSZE INSTRUKCJE SA TU TYLKO TYMCZASOWO - TRZEBA JE WKLEIC NA SWITCHA SKLADNIOWEGO...
	//---- tego jeszcze nie jestem pewien - nie do konca zrozumialem TMa
	
	treeNodeType = tree->type();
	if (treeNodeType == "name") {
		name = tree->getName();
		if (tr->getRoots(name, vec) != 0) {
		fprintf(stderr, "Error in getRoots\n");
		};
		result = new QueryResult;
		vecSize = vec->size();
		for(int i = 0; i < vecSize; i++ ) {
   			optr = vec->at(i);
			lid = optr->getLogicalID();
			result->addNext(lid);	
 		};	
		
		return 0;
	}
	else if (treeNodeType == "create") {
			name = tree->getName();
			value = ((tree->getArg())->getValue());
			if (tr->createObject(name, value, optr) != 0) {
			fprintf(stderr, "Error in createObject\n");
			};
			if (tr->addRoot(optr) != 0) {
			fprintf(stderr, "Error in addRoot\n");
			};
			result = new QueryResult;
			return 0;
		}
		else {
			fprintf(stderr, "QueryExecutor method: TreeNode type unknown\n");
			return -1;
		}
	
	/*switch (cosOdParsera) {
	    case 0: { //create object 
		if (tr->createObject(name, value, optr) != 0) {
		fprintf(stderr, "Error in createObject\n");}
		break;}
	    case 1: { //create object bez wartosci
		if (tr->createObject(name, NULL, optr) != 0) {
		fprintf(stderr, "Error in createObject\n");
		}
		break;}
	    case 2: { //select costam
	    	if (tr->getObjectPointer(lid, mode, optr) != 0) {
		fprintf(stderr, "Error in getObjectPointer\n");
		}

	    break;}
	    default: { break;}
	    }
	*/
	
	
	//---- odpowiednik wywolania CREATE name(value);
	//---- optr jest *wynikiem* tej metody!

	
	//---- odpowiednik wywolania CREATE name; (z pusta wartoscia)
	//---- optr jest *wynikiem* tej metody!
	//KONIEC
	// !!!!! optr - tu jest wynik naszego zapytania (musimy zwrocic jego LogicalID* - funkcja optr.getLogicalID() )
	
	//tutaj ladnie ukladamy QueryResult
}

QueryExecutor::~QueryExecutor() {}
}
