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
  AccessMode *mode;
  ObjectPointer *optr;
  
  fprintf(stderr, "QueryExecutor method: queryResult\n");
  fprintf(stderr, "QueryExecutor asking TransactionManager for a new transaction\n");
  if (TransactionManager::createTransaction(tr) != 0) {
    fprintf(stderr, "Error in createTransaction\n");
  }
  fprintf(stderr, "QueryExecutor asking Store for proxy object to calculate Query\n");
  mode = new AccessMode;
  if (tr->getObjectPointer(lid, *mode, optr) != 0) {
    fprintf(stderr, "Error in getObject\n");
  }
  //tutaj przerabiamy to, co dostalismy od Store na QueryResult
  result = new QueryResult;
  return 0;
}

int QueryExecutor::init(TransactionManager *t) { return 0; }

QueryExecutor::~QueryExecutor() {}

}
