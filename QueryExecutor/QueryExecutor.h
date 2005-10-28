namespace QueryExecutor
{
  class QueryExecutor {
  public:
    virtual QueryExecutor();
    virtual ~QueryExecutor();
    virtual int queryResult(QueryTree *tree, QueryResult *result);
    virtual int init(TransactionManager *t);
    // jakies operatory?
  };
}
