/* Query Executor
   10.2005 - 01.2006

   Authors:
   Beata Golichowska
   Dariusz Gryglas
   Adam Michalik
   Maja Perycz */
   
#include "QueryResult.h"   
#include "TransactionManager/transaction.h"
#include "Store/Store.h"
#include "QueryTree.h"
#include "Proxy.h"

class QueryExecutor {
  
  TransactionManager tm;
  Store store;
  
  public:
    QueryExecutor(TransactionManager& t, Store& s);
    ~QueryExecutor() {};
    int queryResult(QueryTree& tree, QuerryResult& result);
    // jakis konstruktor i destruktor? jakies operatory?
};

QueryExecutor::QueryExecutor(TransactionManager& t, Store& s) {
  tm = t;
  store = s;
}

int QueryExecutor::queryResult(QueryTree& tree, QueryResult& result) {
  int transactionID;
  Proxy storeResult;
  // i byc moze jeszcze ta zmienna mowiaca o id_klienta powinna w nag³owku byæ ale póki co zostawiam tak...
  cout << "QueryExecutor method: queryResult\n";
  cout << "QueryExecutor asking TransactionManager for transaction Number\n";
  transactionID = tm.begin;
  cout << "QueryExecutor asking Store for proxy object to calculate Query\n";
  storeResult = store.getProxy();
  //tutaj przerabiamy to, co dostalismy od Store na QueryResult
  result = new QueryResult;
  return 0;
}
