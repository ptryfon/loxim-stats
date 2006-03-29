
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
    NormalizedQuery::NormalizedQuery(TreeNode*) {
    }

/*______QueryCacher__________________________________________*/
    int QueryCacher::isCacheable(NormalizedQuery* n) {
	return false;
    }

    int QueryCacher::getFromCache(NormalizedQuery* n, QueryResult* &r) {
	return 0;
    }

    QueryCacher::QueryCacher() {
    }

}
