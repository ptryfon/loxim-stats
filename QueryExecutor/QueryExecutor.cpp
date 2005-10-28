/* Query Executor
   10.2005 - 01.2006

   Authors:
   Beata Golichowska
   Dariusz Gryglas
   Adam Michalik
   Maja Perycz */
   
#include <stdio.h>
   
#include "QueryResult.h"   
#include "transaction.h"
#include "Store.h"
#include "QueryTree.h"
#include "Proxy.h"

class QueryExecutor {
  
  TransactionManager tm;
  Store store;
  
  public:
    QueryExecutor();
    ~QueryExecutor() {};
    int queryResult(QueryTree *tree, QuerryResult *result);
    int init(TransactionManager *t);
    // jakies operatory?
};

QueryExecutor::QueryExecutor() {}

int QueryExecutor::init(TransactionManager *t) {
  tm = t;
  return 0;
}

int QueryExecutor::queryResult(QueryTree *tree, QueryResult *result) {
  Transaction tr;
  Proxy res;
  LogicalID lID;
  AccessMode mode;
  
  
  // i byc moze jeszcze ta zmienna mowiaca o id_klienta powinna w nag³owku byæ ale póki co zostawiam tak...
  fprintf(stderr, %s, "QueryExecutor method: queryResult\n");
  fprintf(stderr, %s, "QueryExecutor asking TransactionManager for a new transaction\n");
  new tr;
  if (tm.createTransaction(&tr) != 0) {
    fprintf(stderr, %s, "Error in createTransaction\n");
  }
  fprintf(stderr, %s, "QueryExecutor asking Store for proxy object to calculate Query\n");
  new lID;
  new mode;
  if (tm.getProxy(lID, mode, res) != 0) {
    fprintf(stderr, %s, "Error in getProxy\n");
  }
  //tutaj przerabiamy to, co dostalismy od Store na QueryResult
  result = new QueryResult;
  return 0;
}
