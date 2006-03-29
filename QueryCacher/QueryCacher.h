#ifndef _QUERY_CACHER_
#define _QUERY_CACHER_

#include <stdio.h>

#include "../Errors/ErrorConsole.h"
#include "../QueryExecutor/QueryResult.h"
#include "../QueryParser/TreeNode.h"

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
	    public:
		    NormalizedQuery(TreeNode*);
	};


	/**
	 *	QueryCacher - manages operations on cache
	 */
	class QueryCacher
	{      
	    private:
	    public:
		    int isCacheable(NormalizedQuery*);
		    int getFromCache(NormalizedQuery*, QueryResult*&);
		    QueryCacher();
	};
};

#endif
