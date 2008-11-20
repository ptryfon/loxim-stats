#include <Indexes/IndexHandler.h>
#include <Errors/Errors.h>
#include <Suits.h>
#include <vector>
#include <check.h>
#include <signal.h>
#include <stdlib.h>
#include <set>
 
using namespace Errors;
using namespace Indexes;
using namespace std;
//class IndexHandler;


string filename = "./index.con";

void setup_indexHandler() {
	int err;
	err = IndexHandler::init(filename);
	fail_if(err, "IndexHandler::init failed, error code: %s", SBQLstrerror(err)->c_str());
}

void teardown_indexHandler() {
	int err;
	err = IndexHandler::destroy();
	fail_if(err, "IndexHandler::destroy failed, error code: %s", SBQLstrerror(err)->c_str());
	err = unlink(filename.c_str());
	test(err, "unlink");
}


START_TEST (init) {
	int err;
 	
 	err = IndexHandler::init("./index.con");
 	
 	fail_if(err, "IndexHandler::init failed, error code: %s", SBQLstrerror(err)->c_str());
 	
 	fail_if(Tester::getDroppedNode() != NEW_NODE, "po 1szym zainicjowaniu dropped node ma zla wartosc");
 	fail_if(Tester::getNextFreeNodeAddr() != 0, "po 1szym zainicjowaniu nextFreeNode ma zla wartosc");
 	
	err = IndexHandler::destroy();
    fail_if(err, "IndexHandler::destroy failed, error code: %s", SBQLstrerror(err)->c_str());
 	
}END_TEST

START_TEST (test_instantiation) {
	IndexHandler::init("./index.con");
	CachedNode* n;
	auto_ptr<LogicalID> lid(new DBLogicalID(5));
	auto_ptr<IndexHandler> ih(new IndexHandler("d", "d", "d", lid.get()));
	//ih->tree = new BTree(new IntComparator(), ih.get());
	ih->getNode(n, NEW_NODE);
	IndexHandler::destroy();
	IndexHandler::init("./index.con");
	fail_if(Tester::getUsedCount());
	fail_if(Tester::getCachedCount());
	IndexHandler::destroy();
}END_TEST

void Tester::testGetFree() {
	CachedNode* node;
	fail_unless(Tester::getFd() > 0);
	auto_ptr<LogicalID> lid(new DBLogicalID(5));
	auto_ptr<IndexHandler> ih(new IndexHandler("d", "d", "d", lid.get()));
	ih->tree = new BTree(new IntComparator(), ih.get());
	ih->getNode(node, NEW_NODE);
	fail_if(Tester::getUsedCount() != 1, "pobranie wezla zle ustawia ilosc zajetych");
	ih->freeNode(node);
	fail_if(Tester::getUsedCount() != 0, "zwalnianie wezla zle ustawia ilosc zajetych");
}

START_TEST (get_free) {
	Tester::testGetFree();
}END_TEST
	
void Tester::testGetNewNode() {
	int i, j;
 	char* pos;
	int err;
 	
	CachedNode* cnode, *cnode2;
	auto_ptr<CachedNode> c1, c2;
	Node* node;
	//IndexHandler::init(filename);
	auto_ptr<LogicalID> lid(new DBLogicalID(5));
	auto_ptr<IndexHandler> ih(new IndexHandler("d", "d", "d", lid.get()));
	ih->tree = new BTree(new IntComparator(), ih.get());
	fail_if(Tester::getCache()->size() != 0);
	IndexHandler::freeNodes_t* nodem = Tester::getFreeMap(ih.get());
	fail_if(nodem->size() != 0, "zly rozmiar mapy wolnych wezlow");
		
	IndexHandler::freeHandlers_t* handlerm = Tester::getFreeMap();
	fail_if(handlerm->size() != 0, "zly rozmiar mapy wolnych handlerow");
	
	mark_point();
	err = ih->getNode(cnode, NEW_NODE);
	fail_if(err, "getNode failed, error code: %s", SBQLstrerror(err)->c_str());
	fail_if((Tester::getCache()->size()) != 1);
	nodem = Tester::getFreeMap(ih.get());
	fail_if(nodem->size() != 0, "zly rozmiar mapy wolnych wezlow");
	
	handlerm = Tester::getFreeMap();
	fail_if(handlerm->size() != 0, "zly rozmiar mapy wolnych handlerow");
	
	c1.reset(cnode);
	err = cnode->getNode(node, 1, NODE_WRITE);
	test(err, "cnode->getNode");
	
	nodem = Tester::getFreeMap(ih.get());
	fail_if(nodem->size() != 0, "zly rozmiar mapy wolnych wezlow");
	
	handlerm = Tester::getFreeMap();
	fail_if(handlerm->size() != 0, "zly rozmiar mapy wolnych handlerow");
	
	fail_if (node->id == NEW_NODE, "nowy wezel dostaje ID == NEW_NODE");
	
	pos = (char*)node;
	
	for (i = 0; i < NODE_SIZE; i++) {
		j = *pos;
		pos++;
	}
	
	mark_point();
	
	unsigned long id1 = node->id;
	
	node->leaf = true;
	err = ih->freeNode(cnode);
	test(err, "freeNode");
	
	nodem = Tester::getFreeMap(ih.get());
	fail_if(nodem->size() != 1, "zly rozmiar mapy wolnych wezlow");
	
	handlerm = Tester::getFreeMap();
	fail_if(handlerm->size() != 1, "zly rozmiar mapy wolnych handlerow");
	
	c1.release();
	
	Node* node2;
	
	err = ih->getNode(cnode2, id1);
	test(err, "getNode(oldNode)");
	fail_if(Tester::getCache()->size() != 1);
	nodem = Tester::getFreeMap(ih.get());
	fail_if(nodem->size() != 0, "zly rozmiar mapy wolnych wezlow");
	
	handlerm = Tester::getFreeMap();
	fail_if(handlerm->size() != 0, "zly rozmiar mapy wolnych handlerow");
	
	c2.reset(cnode2);
	
	mark_point();
	err = cnode2->getNode(node2, 1, NODE_READ);
	test(err, "cnode->getNode");
	fail_if(Tester::getCache()->size() != 1);
	nodem = Tester::getFreeMap(ih.get());
	fail_if(nodem->size() != 0, "zly rozmiar mapy wolnych wezlow");
	
	handlerm = Tester::getFreeMap();
	fail_if(handlerm->size() != 0, "zly rozmiar mapy wolnych handlerow");
	
	fail_unless (node2->id == id1, "getNode. wczytywany wezel ma zle id");

	fail_unless (node2->leaf, "getNode. wezel nie pamieta ze jest lisciem");

	ih->getNode(cnode, NEW_NODE);
	fail_if(Tester::getCache()->size() != 2);
	nodem = Tester::getFreeMap(ih.get());
	fail_if(nodem->size() != 0, "zly rozmiar mapy wolnych wezlow");
	
	handlerm = Tester::getFreeMap();
	fail_if(handlerm->size() != 0, "zly rozmiar mapy wolnych handlerow");
	
	c1.reset(cnode);
	cnode->getNode(node, 2, NODE_WRITE);
	fail_if(Tester::getCache()->size() != 2);
	fail_if(id1 == node->id, "nowe wezly zwracane z blednym id");
    
    err = ih->freeNode(cnode2);
	test(err, "freeNode");
	fail_if(Tester::getCache()->size() != 2);
	nodem = Tester::getFreeMap(ih.get());
	fail_if(nodem->size() != 1, "zly rozmiar mapy wolnych wezlow");
	
	handlerm = Tester::getFreeMap();
	fail_if(handlerm->size() != 1, "zly rozmiar mapy wolnych handlerow");
	
	c2.release();
	
	err = ih->freeNode(cnode);
	test(err, "freeNode");
	
	fail_if(Tester::getCache()->size() != 2);
	nodem = Tester::getFreeMap(ih.get());
	fail_if(nodem->size() != 2, "zly rozmiar mapy wolnych wezlow");
	
	handlerm = Tester::getFreeMap();
	fail_if(handlerm->size() != 1, "zly rozmiar mapy wolnych handlerow: %d", err);
	
	c1.release();
	
    fail_if(Tester::getUsedCount() != 0, "zwalnianie wezla zle ustawia ilosc zajetych");
    fail_if(Tester::getCache()->size() != 2);
}

START_TEST (get_new_node) {
	Tester::testGetNewNode();
 	
}END_TEST

void Tester::testGetNodeLock() {
	int err;
	CachedNode* node;
	auto_ptr<LogicalID> lid(new DBLogicalID(5));
	auto_ptr<IndexHandler> ih(new IndexHandler("d", "d", "d", lid.get()));
	//trzeba to odkomentowac jesli chcemy ten test odpalic
	ih->tree = new BTree(new IntComparator(), ih.get());
	auto_ptr<CachedNode> c1;
	
	for (uint i = 0; i < MAX_CACHED_NODES+1; i++) {
		
		err = ih->getNode(node, NEW_NODE, true);
		test(err,"getNode()");
		c1.reset(node);
	}
}

//rzuca bledem bo zapetlenie
START_TEST (get_node_lock) {
	Tester::testGetNodeLock();
}END_TEST

void Tester::testGetFreeSwap() {
	int err;
	CachedNode* cnode;
	vector<CachedNode*> v;
	auto_ptr<LogicalID> lid(new DBLogicalID(5));
	auto_ptr<IndexHandler> ih(new IndexHandler("d", "d", "d", lid.get()));
	ih->tree = new BTree(new IntComparator(), ih.get());
	//err = IndexHandler::init("./index.con");
	
 	//fail_if(err, "IndexHandler::init failed, error code: %s", SBQLstrerror(err)->c_str());
	
	fail_if(Tester::getUsedCount() != 0, "pobranie wezla zle ustawia ilosc zajetych");
	
	IndexHandler::freeNodes_t* nodem = Tester::getFreeMap(ih.get());
	fail_if(nodem->size() != 0, "zly rozmiar mapy wolnych wezlow");
		
	IndexHandler::freeHandlers_t* handlerm = Tester::getFreeMap();
	fail_if(handlerm->size() != 0, "zly rozmiar mapy wolnych handlerow");
	
	for (uint i = 0; i < MAX_CACHED_NODES; i++) {
		
		err = ih->getNode(cnode, NEW_NODE, true);
		test(err,"getNode()");
		v.push_back(cnode);
	}
	fail_if(Tester::getUsedCount() != MAX_CACHED_NODES, "pobranie wezla zle ustawia ilosc zajetych");
	cnode = v.back();
	
	Node* node;
	cnode->getNode(node, 1, NODE_WRITE);
	
	err = ih->getNode(cnode, node->id, true); //pobranie wezla ktory jest w cache'u
	test(err,"getNode()");
	
	fail_if(Tester::getUsedCount() != MAX_CACHED_NODES, "pobranie wezla zle ustawia ilosc zajetych");
	
	err = ih->freeNode(cnode); // zwolnienie tego ktory jest w cache'u
	test(err, "freeNode");
	
	fail_if(Tester::getUsedCount() != MAX_CACHED_NODES, "pobranie wezla zle ustawia ilosc zajetych");
	
	err = ih->freeNode(cnode); //teraz jednego wezla z cache'u nikt nie uzywa
	test(err, "freeNode");
	
	fail_if(Tester::getUsedCount() != MAX_CACHED_NODES-1, "pobranie wezla zle ustawia ilosc zajetych");
	
	err = ih->getNode(cnode, NEW_NODE, true); //pobranie wezla i swap innego wezla
	test(err,"getNode()");
	
	fail_if(Tester::getUsedCount() != MAX_CACHED_NODES, "pobranie wezla zle ustawia ilosc zajetych");
	
	//err = IndexHandler::destroy();
    //fail_if(err, "IndexHandler::destroy failed, error code: %s", SBQLstrerror(err)->c_str());
			
}

START_TEST (get_free_swap) {
	Tester::testGetFreeSwap();
}END_TEST

void Tester::testSwappingInfo() {
	Node* node;
	CachedNode* cnode;
	int err;
	auto_ptr<LogicalID> lid(new DBLogicalID(5));
	auto_ptr<IndexHandler> ih(new IndexHandler("d", "d", "d", lid.get()));
	ih->tree = new BTree(new IntComparator(), ih.get());
	
	IndexHandler::init("./index.con");
	
	char pattern[NODE_SIZE];
	int i;
	for (i = 0; i < NODE_SIZE/2; i++) {
		pattern[i] = (unsigned int) rand();
	}
	for (;i < NODE_SIZE; i++) {
		pattern[i] = 53;
	}
	
	set<nodeAddress_t> myset;
	
	//showmem(&pattern);
	//const char* txt = IndexHandler::printHandlers(); 
	//cout <<  txt << endl;
			
	for (uint j = 0; j < 2 * MAX_CACHED_NODES; j++) {
	
		node = NULL;
		mark_point();
		err = ih->getNode(cnode, NEW_NODE, true);
		test(err,"getNode()");
		
		//txt = IndexHandler::printHandlers(); 
		//cout <<  txt << endl;
		
		err = cnode->getNode(node, 1, NODE_WRITE);
		test(err, "cnode->getNode");
		
		int count = myset.count(node->id);
		
		fail_if(count, "podano stary wezel jako nowy");
		
		myset.insert(node->id);
		
		memcpy(node->afterHeader(), &pattern, node->left);
		err = ih->freeNode(cnode);
		test(err, "freeNode");
		
		//txt = IndexHandler::printHandlers(); 
		//cout <<  txt << endl;
	}
	fail_if(Tester::getCachedCount() != MAX_CACHED_NODES, "powinno byc zcacheowanych %d wezlow a jest %d", MAX_CACHED_NODES, Tester::getCachedCount());
	mark_point();
	nodeAddress_t swapped = NEW_NODE;
	//kilka wezlow musialo zostac zcacheowanych
	set<nodeAddress_t>::iterator it;
	for ( it=myset.begin() ; it != myset.end(); it++ ) {
		if (!Tester::cacheLookup(*it)) {
			//nie znalazl wezla w cache'u
			swapped = *it;
			break;
		}		
	}
	
	fail_if(swapped == NEW_NODE, "nie znaleziono zeswapowanego wezla");
	
	err = ih->getNode(cnode, swapped, true);
	test(err,"getNode()");
	
	cnode->getNode(node, 1, NODE_WRITE);
	
	err = memcmp(node->afterHeader(), &pattern, node->left);
	
	fail_if(err, "wyjety wezel rozni sie od wlozonego");
	
	err = ih->freeNode(cnode);
	test(err, "freeNode");	
}

START_TEST (swapping_info) {
	Tester::testSwappingInfo();
}END_TEST

void Tester::testPrioritySwap() {
	int err;
	uint count;
	//IndexHandler::init(filename);
	auto_ptr<LogicalID> lid(new DBLogicalID(5));
	auto_ptr<IndexHandler> ih1(new IndexHandler("d", "d", "d", lid.get()));
	auto_ptr<IndexHandler> ih2(new IndexHandler("e", "e", "e", lid.get()));
	ih1->tree = new BTree(new IntComparator(), ih1.get());
	ih2->tree = new BTree(new IntComparator(), ih2.get());
	CachedNode* cnode;
	Node* node;
	nodeAddress_t id;
	//IndexHandler *ih;
	ih1->tree->rootLevel = 2 * MAX_CACHED_NODES;
	ih2->tree->rootLevel = 2 * MAX_CACHED_NODES;
	for (uint i = 1; i <= MAX_CACHED_NODES - 2; i++) {
		//ih = (rand() % 2) ? ih1.get() : ih2.get(); //wybieramy jednego z indexHandler'ow		
		
		err = ih1->getNode(cnode, NEW_NODE, true);
		cnode->getNode(node, i, NODE_WRITE);
		ih1->freeNode(cnode);
	}
	err = ih1->getNode(cnode, NEW_NODE, true);
	err = cnode->getNode(node, 0, NODE_WRITE);
	id = node->id;
	ih1->freeNode(cnode);
	
	fail_if(IndexHandler::cache.size() != MAX_CACHED_NODES - 1);
	
	err = ih2->getNode(cnode, NEW_NODE, true);
	cnode->getNode(node, 2, NODE_WRITE);
	ih2->freeNode(cnode);
	
	//teraz caly cache jest uzywany
	count = Tester::getCachedCount();
	fail_if(count != MAX_CACHED_NODES, "powinno byc scache'owanych MAX wezlo a jest %d", err);
	
	err = ih2->getNode(cnode, NEW_NODE, true);
	cnode->getNode(node, 2, NODE_WRITE);
	ih2->freeNode(cnode);
	//powinien zostac zwolniony wezel id
	
	IndexHandler::cacheMapT::iterator it;
	for (it = Tester::getCache()->begin(); it != Tester::getCache()->end(); it++) {
		fail_if (it->first == id, "tego wezla nie powinno bycw cache'u");
	}
		
}

START_TEST (priority_swap) {
	Tester::testPrioritySwap();
}END_TEST

void Tester::testManyTreeSwap() {
	int err;
	//uint count;
	//IndexHandler::init(filename);
	auto_ptr<LogicalID> lid(new DBLogicalID(5));
	auto_ptr<IndexHandler> ih1(new IndexHandler("d", "d", "d", lid.get()));
	auto_ptr<IndexHandler> ih2(new IndexHandler("e", "e", "e", lid.get()));
	ih1->tree = new BTree(new IntComparator(), ih1.get());
	ih2->tree = new BTree(new IntComparator(), ih2.get());
	CachedNode* cnode;
	Node* node;
	nodeAddress_t id;
	//IndexHandler *ih;
	ih1->tree->rootLevel = 2*MAX_CACHED_NODES;
	ih2->tree->rootLevel = (MAX_CACHED_NODES / 2) + 1;
	map<uint, nodeAddress_t> m1, m2;
	for (uint i = 0; i < 2*MAX_CACHED_NODES ; i++) {
		//ih = (rand() % 2) ? ih1.get() : ih2.get(); //wybieramy jednego z indexHandler'ow		
		
		err = ih1->getNode(cnode, NEW_NODE, true);
		cnode->getNode(node, i, NODE_WRITE);
		m1[i] = node->id;
		ih1->freeNode(cnode);
		
		if (i < MAX_CACHED_NODES / 2) {
			err = ih2->getNode(cnode, NEW_NODE, true);
			cnode->getNode(node, i, NODE_WRITE);
			m2[i] = node->id;
			ih2->freeNode(cnode);
		}
	}
	mark_point();
	
	//cache jest pelny. sa w nim wezly blisko korzenia
	//ih2 jest 4x nizszy niz ih1. wiec 2 z h1 powinien byc zwolniony wczesniej niz 1 z h2
	
	err = ih2->getNode(cnode, m2[1], true);
	cnode->getNode(node, 1, NODE_READ);
	ih2->freeNode(cnode);
	
	err = ih1->getNode(cnode, m1[2], true);
	cnode->getNode(node, 2, NODE_READ);
	ih1->freeNode(cnode);
	
	
	fail_if(IndexHandler::cache.size() != MAX_CACHED_NODES);
	
	bool occured1 = false, occured2 = false;
	IndexHandler::cacheMapT::iterator it;
	//w cache'u nie powinno byc wezla z ih1 z poziomu MAX_CACHED_NODES
	id = m1[MAX_CACHED_NODES];
	for (it = IndexHandler::cache.begin(); it != IndexHandler::cache.end(); it++) {
		fail_if(it->first == id);
		fail_if(it->first == m1[0]);
		fail_if(it->first == m1[1]);
		fail_if(it->first == m2[0]);
		//fail_if(it->first == m2[2]);
		
		if (it->first == m1[2]) {
			occured1 = true;
		}
		if (it->first == m2[1]) {
			occured2 = true;
		}
	}
	
	fail_unless(occured1);
	fail_unless(occured2);
	//w cache'u jest m1[2], m2[1] i nie ma id o priorytecie MAX_CACHED_NODES
	
	mark_point();
	
	err = ih1->getNode(cnode, id, true);
	cnode->getNode(node, MAX_CACHED_NODES, NODE_READ);
	ih1->freeNode(cnode);
	
	//powinien zostac wybrany wezel najglebszy czyli 2 z h1, czyli m1[2]
	
	for (it = IndexHandler::cache.begin(); it != IndexHandler::cache.end(); it++) {
		fail_if(it->first == m1[2]);
	}
	
	//w cache'u jest m1[MAX_CACHED_NODES], m2[1], i nie ma m1[2]
	CachedNode *cnode2;
	Node* node2;
	
	err = ih2->getNode(cnode2, m2[1], true);
	cnode2->getNode(node2, 1, NODE_READ);
	
	err = ih1->getNode(cnode, m1[0], true);
	cnode->getNode(node, 0, NODE_READ);
	ih1->freeNode(cnode);
	
	ih2->freeNode(cnode2);
	
	occured1 = occured2 = false;
	//powinien zostac splukany m1[MAX_CACHED_NODES]
	for (it = IndexHandler::cache.begin(); it != IndexHandler::cache.end(); it++) {
		fail_if(it->first == m1[MAX_CACHED_NODES]);
		if (it->first == m2[1]) {
			occured1 = true;
		}
		if (it->first == m1[0]) {
			occured2 = true;
		}
	}
	fail_unless(occured1 && occured2);
	
	//w cache'u jest m2[1]. m1[0] i nie ma m1[MAX_CACHED_NODES]
	
	err = ih1->getNode(cnode, m1[MAX_CACHED_NODES], true);
	cnode->getNode(node, MAX_CACHED_NODES, NODE_READ);
	ih1->freeNode(cnode);
	
	//powinien zostac zwolniony m1[0] bo jest wyzszy
	for (it = IndexHandler::cache.begin(); it != IndexHandler::cache.end(); it++) {
		fail_if(it->first == m1[0]);
	}
}

START_TEST (many_tree_swap) {
	Tester::testManyTreeSwap();
}END_TEST

void Tester::testGetDropped() {
	auto_ptr<LogicalID> lid(new DBLogicalID(5));
	auto_ptr<IndexHandler> ih1(new IndexHandler("d", "d", "d", lid.get()));
	auto_ptr<IndexHandler> ih2(new IndexHandler("e", "e", "e", lid.get()));
	ih1->tree = new BTree(new IntComparator(), ih1.get());
	ih2->tree = new BTree(new IntComparator(), ih2.get());
	//int err = 0;
	CachedNode* cnode, *cnode2;
	Node* node, *node2;
	
	ih2->getNode(cnode, NEW_NODE, true);
	cnode->getNode(node, 0, NODE_WRITE);
	nodeAddress_t id1 = node->id;
	
	ih2->getNode(cnode2, NEW_NODE, true);
	cnode2->getNode(node2, 0, NODE_WRITE);
	nodeAddress_t id2 = node2->id;
	
	fail_if(IndexHandler::droppedCachedNodes.size() != 0);
	ih2->dropNode(cnode2);
	fail_if(IndexHandler::droppedCachedNodes.size() != 1);
	ih2->dropNode(cnode);
	fail_if(IndexHandler::droppedCachedNodes.size() != 2);
	
	ih1->getNode(cnode, NEW_NODE, true);
	cnode->getNode(node, 0, NODE_WRITE);
	
	fail_if(node->id != id1, "zwrocono nie ten wezel. zwrocono %d, oczekiwano %d", node->id, id1);
	fail_if(IndexHandler::droppedCachedNodes.size() != 1);
	
	ih1->getNode(cnode, NEW_NODE, true);
	cnode->getNode(node, 0, NODE_WRITE);
	
	fail_if(node->id != id2, "zwrocono nie ten wezel. zwrocono %d, oczekiwano %d", node->id, id2);
	
	fail_if(IndexHandler::droppedCachedNodes.size() != 0);
}

START_TEST (get_dropped) {
	Tester::testGetDropped();
}END_TEST

void Tester::testGetDroppedFlush() {
	auto_ptr<LogicalID> lid(new DBLogicalID(5));
	auto_ptr<IndexHandler> ih1(new IndexHandler("d", "d", "d", lid.get()));
	auto_ptr<IndexHandler> ih2(new IndexHandler("e", "e", "e", lid.get()));
	ih1->tree = new BTree(new IntComparator(), ih1.get());
	ih2->tree = new BTree(new IntComparator(), ih2.get());
	int err = 0;
	CachedNode* cnode;
	Node* node;
	
	fail_if(IndexHandler::cache.size() != 0);
	ih2->getNode(cnode, NEW_NODE, true);
	fail_if(IndexHandler::cache.size() != 1);
	cnode->getNode(node, 0, NODE_WRITE);
	fail_if(IndexHandler::cache.size() != 1);
	//jeden wezel zostal wczytany
	
	//wczytujemy kolejne MAX_CACHED_NODES - 1
	nodeAddress_t id;
	for (unsigned int i = 1; i < MAX_CACHED_NODES; i++) {
		ih2->getNode(cnode, NEW_NODE, true);
		cnode->getNode(node, 0, NODE_WRITE);
		id = node->id;
	}
	fail_if(IndexHandler::cache.size() != MAX_CACHED_NODES);
	//caly cache zajety
	//zwalniamy jedno miejsce
	ih2->freeNode(cnode);
	
	fail_if(IndexHandler::cache.size() != MAX_CACHED_NODES);
	
	//wczytuje nowy wezel
	ih2->getNode(cnode, NEW_NODE, true);
	cnode->getNode(node, 0, NODE_WRITE);
	//caly cache zajety
	fail_if(IndexHandler::cache.size() != MAX_CACHED_NODES);
	
	nodeAddress_t droppedId = node->id;
	fail_if (IndexHandler::droppedCachedNodes.size() != 0);
	ih2->dropNode(cnode);
	
	fail_if(IndexHandler::cache.size() != MAX_CACHED_NODES, "w cache'u powinno byc %d a jest %d wezlow", MAX_CACHED_NODES, IndexHandler::cache.size());
	
	fail_if (IndexHandler::droppedNode != droppedId);
	fail_if (IndexHandler::droppedCachedNodes.size() != 1);
	//jedno wolne miejsce
	//pobieram wezel o danym adresie. czy bedzie deadlock?
	err = ih2->getNode(cnode, id, true);
	fail_if (IndexHandler::droppedCachedNodes.size() != 0);
	cnode->getNode(node, 0, NODE_READ);
	
	fail_if (IndexHandler::droppedCachedNodes.size() != 0);
	fail_if(IndexHandler::cache.size() != MAX_CACHED_NODES);
	
	IndexHandler::flushAll();

}

START_TEST (get_dropped_flush) {
	Tester::testGetDroppedFlush();
}END_TEST

void Tester::testBeyondCacheSize() {
	auto_ptr<LogicalID> lid(new DBLogicalID(5));
	auto_ptr<IndexHandler> ih1(new IndexHandler("d", "d", "d", lid.get()));
	auto_ptr<IndexHandler> ih2(new IndexHandler("e", "e", "e", lid.get()));
	ih1->tree = new BTree(new IntComparator(), ih1.get());
	ih2->tree = new BTree(new IntComparator(), ih2.get());
	int err = 0;
	CachedNode* cnode;
	Node* node;
	
	for (unsigned int i = 0; i < MAX_CACHED_NODES; i++) {
		err = ih2->getNode(cnode, NEW_NODE, true);
		cnode->getNode(node, 0, NODE_WRITE);
	}
	fail_if(IndexHandler::cache.size() != MAX_CACHED_NODES);
	ih1->getNode(cnode, NEW_NODE, false);
	cnode->getNode(node, 0, NODE_WRITE);
	fail_if(IndexHandler::cache.size() != MAX_CACHED_NODES + 1);
	ih1->freeNode(cnode);
	fail_if(IndexHandler::cache.size() != MAX_CACHED_NODES);
}

START_TEST (beyond_cache_size) {
	Tester::testBeyondCacheSize();
}END_TEST

/*
START_TEST (segfault) {
	
	//while(true){};
	//fail("drugi test");
	char* p = NULL;
	(*p)++;
}END_TEST
*/

/** template

START_TEST (name) {
}END_TEST

*/

Suite * node_buffer () {
	Suite *s = suite_create ("NodeBuffer");
	
	TCase *initialization = tcase_create ("Initialization");
	tcase_add_test (initialization, test_instantiation);
	tcase_add_test (initialization, init);
	
	TCase *tc_core = tcase_create ("Node Request");
	tcase_add_checked_fixture(tc_core, setup_indexHandler, teardown_indexHandler);
	
	tcase_add_test (tc_core, get_free);
    tcase_add_test (tc_core, get_new_node);
    tcase_add_test (tc_core, swapping_info);
    
   // tcase_add_test (tc_core, loop);
	//tcase_add_test_raise_signal(tc_core, segfault, SIGSEGV);
    suite_add_tcase (s, tc_core);
    
    TCase *tc_lock = tcase_create ("BufferLock");
    tcase_add_checked_fixture(tc_lock, setup_indexHandler, teardown_indexHandler);
	tcase_set_timeout(tc_lock, 1); //2 sekundy
	tcase_add_test_raise_signal(tc_lock, get_node_lock, SIGKILL);
	//tcase_add_test (tc_lock, get_node_lock);
	//suite_add_tcase (s, tc_lock);
	
	
	TCase *tc_swap = tcase_create ("Swap");
	tcase_set_timeout(tc_swap, 4); //2 sekundy
	tcase_add_checked_fixture(tc_swap, setup_indexHandler, teardown_indexHandler);
	tcase_add_test (tc_swap, get_free_swap);
	tcase_add_test (tc_swap, priority_swap);
	tcase_add_test (tc_swap, get_dropped);
	tcase_add_test (tc_swap, get_dropped_flush);
	tcase_add_test (tc_swap, beyond_cache_size);
	tcase_add_test (tc_swap, many_tree_swap);
	
	suite_add_tcase (s, tc_swap);
	return s;
}


