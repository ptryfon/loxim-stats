#include "../BTree.h"
#include "../Node.h"
#include "../IndexHandler.h"
#include "../QueryOptimizer.h"

namespace Indexes {

	class Tester {
		public:
			static bool debugMode;
			
			static nodeAddress_t getRoot(BTree* t);
			static unsigned long getUsedCount();
			static unsigned long getCachedCount();
			static int addEntry(BTree* t, CachedNode* n, Comparator* c);
			static nodeAddress_t getNextFreeNodeAddr();
			static nodeAddress_t getDroppedNode();
			static int getFd();
			static nodeEntry_off_t getStrSize(Comparator* c);
			static CachedNode* cacheLookup(nodeAddress_t nodeID);
			static bool isDebugMode();
			static RootEntry* getEntry(Comparator* c);
			static void whereReplace(QueryOptimizer* opt, TreeNode* tNode);
			static bool optimizationPossible(QueryOptimizer* opt);
			static int commitMapSize(VisibilityResolver* v);
			static IndexHandler::freeNodes_t* getFreeMap(IndexHandler* ih);
			static IndexHandler::freeHandlers_t* getFreeMap();
			static IndexHandler::cacheMapT* getCache();
			static VisibilityResolver::rolledBack_t getRolledBack();
			static VisibilityResolver::active_t getActive();
			static VisibilityResolver::commitTs_t getCommitTs();
			static tid_t getTransactionId();
			static IndexManager::string2index* getIndexNames();
			static BTree* getTree(IndexHandler* ih);
			static int getPriority(BTree *tree);
			
			static void testGetDroppedFlush();
			static void testGetDropped();
			static void testIndexCount();
			static void testBeyondCacheSize();
			static void testSplit();
			static void getIndexNodeList();
			static void getNode(vector<nodeAddress_t> &list, nodeAddress_t addr, IndexHandler *ih);
			static void deleteNode(vector<nodeAddress_t> &list, IndexHandler* ih, int pos);
			static void checkIndexList(vector<nodeAddress_t>& list, IndexHandler* ih);
			static void testDroppedList();
			static void testDropIndex();
			static void testManyTreeSwap();
			static void testGetNewNode();
			static void testGetFreeSwap();
			static void testSwappingInfo();
			static void testPrioritySwap();
			static void testGetFree();
			static void testGetNodeLock();
			static void testRootDrop();
			static void testCreateInt();
			static void testAdding();
			static void testSearching();
			static void testBigTree();
			static void testExactInt();
			static void testBadNode();
			static void testRollback();
			static void testBigTreeString();
			static void testBigTreeDouble();
			static void addRollback();
			static void visibilityMap();
			static void testMultiRollback();
	};

}
