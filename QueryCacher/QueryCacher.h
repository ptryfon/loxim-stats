#ifndef _QUERY_CACHER_
#define _QUERY_CACHER_

#include <stdio.h>

#include "../Errors/ErrorConsole.h"
#include "../QueryExecutor/QueryResult.h"
#include "../QueryParser/TreeNode.h"
#include "../Errors/ErrorConsole.h"
#include "../Config/SBQLConfig.h"
#include "../TransactionManager/SemHeaders.h"
#include "CacheMap.h"
#include "QueryCacherConfig.h"


using namespace Errors;
using namespace QExecutor;
using namespace QParser;


//first aproach to query caching
namespace QCacher
{
	/**
	 *	CacheID
	 */
	class CacheID
	{
	    private:
		    int id;
	    public:
		    int getId();
		    CacheID(int);
	};


	/**
	 *	NormalizedQuery - query shape for equvalence relation (or partial order)
	 */
	class NormalizedQuery
	{
	    private:
		    TreeNode* query;
	    public:
		    NormalizedQuery(TreeNode*);
		    ~NormalizedQuery();
	};


	/**
	 *	QueryCacher - manages operations on cache
	 */
	class QueryCacher
	{      
	    private:
		    int slots;
		    bool use_cache;
		    Mutex* mutex;
		    
		    AbstractCacheMap* cache;
		    
		    
		    ErrorConsole err;
		    static QueryCacher* qCacher;
		    int loadConfig();
	    
		    int isCacheable(TreeNode*);
		    QueryCacher();
	    public:
		    ~QueryCacher();
		    static QueryCacher* getHandle();
	    
		    int get(TreeNode*, QueryResult*&);
		    int put(TreeNode*, QueryResult*);
		    int remove(TreeNode*);
		    int unstampTransaction(int);
	};
};

#endif
