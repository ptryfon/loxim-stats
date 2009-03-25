#include <Test/Indexes/Tester.h>

namespace Indexes {
	
	bool Tester::debugMode = false;

	nodeAddress_t Tester::getRoot(BTree* t) {
		return t->rootAddr;
	}
	
	unsigned long Tester::getUsedCount() {
		return IndexHandler::usedCount;
	}
	
	unsigned long Tester::getCachedCount() {
		return IndexHandler::cachedCount;
	}
	
	int Tester::addEntry(BTree* t, CachedNode* n, Comparator* c) {
		return t->addEntry(n, c);
	}

	nodeAddress_t Tester::getDroppedNode() {
		return IndexHandler::droppedNode;
	}
	
	nodeAddress_t Tester::getNextFreeNodeAddr() {
		return IndexHandler::nextFreeNodeAddr;
	}
	
	int Tester::getFd() {
		return IndexHandler::index_fd;
	}
	
	nodeEntry_off_t Tester::getStrSize(Comparator* c) {
		return (dynamic_cast<StringComparator*> (c))->strSize;
	}
	
	CachedNode* Tester::cacheLookup(nodeAddress_t nodeID) {
		return IndexHandler::cacheLookup(nodeID);
	}
	
	bool Tester::isDebugMode() {
		return debugMode;
	}
	
	RootEntry* Tester::getEntry(Comparator* c) {
		return c->rootEntry;
	}
	
	void Tester::whereReplace(QueryOptimizer* opt, TreeNode* tNode) {
		opt->whereReplace(tNode);
	}
	
	bool Tester::optimizationPossible(QueryOptimizer* opt) {
		return opt->optimizationPossible();
	}
	
	int Tester::commitMapSize(VisibilityResolver* v) {
		return v->commitTs.size();
	}
	
	IndexHandler::freeNodes_t* Tester::getFreeMap(IndexHandler* ih) {
		return &(ih->freeNodes);
	}
	
	IndexHandler::freeHandlers_t* Tester::getFreeMap() {
		return &(IndexHandler::freeHandlers);
	}
	
	IndexHandler::cacheMapT* Tester::getCache() {
		return &IndexHandler::cache;
	}
	
	VisibilityResolver::rolledBack_t Tester::getRolledBack() {
		return IndexManager::getHandle()->getResolver()->rolledBack;
	}
	
	VisibilityResolver::active_t Tester::getActive() {
		return IndexManager::getHandle()->getResolver()->active;
	}
	
	VisibilityResolver::commitTs_t Tester::getCommitTs() {
		return IndexManager::getHandle()->getResolver()->commitTs;
	}
	
	tid_t Tester::getTransactionId() {
		return TransactionManager::getHandle()->transactionId;
	}
	
	IndexManager::string2index* Tester::getIndexNames() {
		return &(IndexManager::getHandle()->indexNames);
	}

	BTree* Tester::getTree(IndexHandler* ih) {
		return ih->tree;
	}
	
	int Tester::getPriority(BTree *tree) {
		return tree->rootPriority();
	}
}

