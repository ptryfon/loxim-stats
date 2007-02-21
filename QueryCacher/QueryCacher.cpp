
#include "QueryCacher.h"

namespace QCacher
{
/*______CacheID______________________________________________*/
    int CacheID::getId() {
	return id;
    }

    CacheID::CacheID(int i) {
	id = i;
    }

/*______NormalizedQuery______________________________________*/
    NormalizedQuery::NormalizedQuery(TreeNode*) {}

    NormalizedQuery::~NormalizedQuery() {}

/*______QueryCacher__________________________________________*/
    QueryCacher::QueryCacher()
    {
	err = ErrorConsole("QueryCacher");
	loadConfig();
//	cache = new StandardHashMap();
	mutex = new Mutex();
	mutex->init();
    }

    QueryCacher::~QueryCacher() {}
    
    QueryCacher* QueryCacher::qCacher = new QueryCacher();
	
    QueryCacher* QueryCacher::getHandle() { return qCacher; };


    int QueryCacher::isCacheable(TreeNode* n) {
	return false;
    }

    int QueryCacher::get(TreeNode* n, QueryResult* &r) {
	r = NULL;
	CacheEntry* entry = new CacheEntry(n, r);

        if (!isCacheable(n))
	    return 0;
	
	if (cache->get(n, entry))
	    return 0;		// Query successfully retrieved from cache
/*	else {
	    n->markCacheable();
	}
*/
	return 0;
    }

    int QueryCacher::put(TreeNode* n, QueryResult* r) {
	CacheEntry* entry = new CacheEntry(n, r);
	if (cache->put(n, entry))
	    return 0;
	else
	    return -1;
    }

    int QueryCacher::remove(TreeNode* n) {
	if (cache->remove(n))
	    return 0;
	else
	    return -2;
    }

    int QueryCacher::unstampTransaction(int tid) {
//	if (cache->
	return 0;
    }

    int QueryCacher::loadConfig()
    {
		slots = 100;
		use_cache = false;
		string cache_string = "on";
		SBQLConfig conf("QueryCacher");

		int errorCode = conf.getInt( "slots", slots);
		if (errorCode)
		    err.printf("Cannot read number of slots from configuration, using default value %d\n", slots);
		errorCode  = conf.getString( "use_cache", cache_string);
		if (cache_string == "on")
		    use_cache = true;
		if (errorCode)
		    err << "Cannot read use_cache switch from configuration, using default value " + cache_string;

		return 0;
    }

}
