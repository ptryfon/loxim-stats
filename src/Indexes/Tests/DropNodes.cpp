#include "Suits.h"
#include "Indexes/Cleaner.h"
#include <unistd.h>

using namespace Errors;
using namespace Indexes;

void Tester::checkIndexList(vector<nodeAddress_t>& list, IndexHandler* ih) {
	nodeAddress_t id;
	CachedNode* cnode;
	Node* node;
	int err = 0;
	int count = list.size();
	for (int i = 0; i < count; i++) {
		id = list.at(i);
		
		err = ih->getNode(cnode, id);
		test(err, "getnode");
		err = cnode->getNode(node, 0, NODE_WRITE);
		test(err, "getnode");
		
		if (i == 0) {
			fail_if(ih->firstNode != id);
			fail_if(node->prevNode != NEW_NODE);
		} else if (i == count-1) {
			fail_if(ih->lastNode != id);
			fail_if(node->nextNode != NEW_NODE);
		} else {
			//wezel w srodku listy
			fail_if(node->nextNode != list.at(i+1));
			fail_if(node->prevNode != list.at(i-1));
		}
		
		err = ih->freeNode(cnode);
		test(err, "freenode");
	}
}

void Tester::getIndexNodeList() {
	int err;
	auto_ptr<LogicalID> lid(new DBLogicalID(5));
	auto_ptr<IndexHandler> ih(new IndexHandler("d", "d", "d", lid.get()));
	auto_ptr<BTree> t(new BTree(new IntComparator(), ih.get()));
	
	fail_if(ih->firstNode != NEW_NODE);
	fail_if(ih->lastNode != NEW_NODE);
	ih->tree = t.get();
	err = t->createRoot();
	test(err, "createRoot");
	
	
	nodeAddress_t first = ih->firstNode;
	nodeAddress_t last = ih->lastNode;
	fail_if(first != last);
	fail_if(first == NEW_NODE);
	
	vector<nodeAddress_t> list;
	//dodac roota
	list.insert(list.begin(), first);
	
	for (unsigned int i = 0; i < 2 * MAX_CACHED_NODES; i++) {
		//pobieramy nowe wezly
		Tester::getNode(list, NEW_NODE, ih.get());
	}
	fail_if(IndexHandler::cache.size() != IndexHandler::cachedCount);
	fail_if(IndexHandler::cache.size() != MAX_CACHED_NODES);
	
	//sprawdzamy czy na dysku jest to samo co w vectorze
	checkIndexList(list, ih.get());
	
	fail_if(IndexHandler::droppedLastNode != NEW_NODE);
	nodeAddress_t toDel = list.at(list.size() - 1);
	//usuwamy pierwszy i ostatni
	deleteNode(list, ih.get(), list.size()-1);
	fail_if(IndexHandler::droppedLastNode != toDel);
	deleteNode(list, ih.get(), 0);
	fail_if(IndexHandler::droppedLastNode != toDel);
	
	int pos;
	int count = list.size() / 2;
	//usuwamy polowe wezlow
	for (int i = 0; i < count; i++) {
		pos = rand() % (list.size());
		deleteNode(list, ih.get(), pos);
		pos = rand() % (list.size());
		//odwiezamy wezly juz zwiazane z indeksem
		Tester::getNode(list, list.at(pos), ih.get());
	}
	
	//czy wszystko sie zgadza
	checkIndexList(list, ih.get());
	
	count = list.size();
	for (int i = 0; i < count; i++) {
		deleteNode(list, ih.get(), 0);
	}
	fail_unless(list.empty());
	
	fail_if(ih->firstNode != NEW_NODE);
	fail_if(ih->lastNode != NEW_NODE);
	
	t.release();
}

void Tester::getNode(vector<nodeAddress_t> &list, nodeAddress_t addr, IndexHandler *ih) {
	int err = 0;
	CachedNode *cnode;
	Node *node;
	err = ih->getNode(cnode, addr);
	test(err, "getnode");
	err = cnode->getNode(node, 0, NODE_WRITE);
	test(err, "getnode");
	if (addr == NEW_NODE) {
		list.insert(list.begin(), node->id);
	}
	err = ih->freeNode(cnode);
	test(err, "freenode");;
}

void Tester::deleteNode(vector<nodeAddress_t> &list, IndexHandler* ih, int pos) {
	nodeAddress_t id = list.at(pos);
	int err = 0;
	CachedNode *cnode;
	Node *node;
	list.erase(list.begin()+pos);
	
	err = ih->getNode(cnode, id);
	test(err, "getnode");
	err = cnode->getNode(node, 0, NODE_WRITE);
	test(err, "getnode");
	
	err = ih->dropNode(cnode);
	test(err, "dropnode");
}

START_TEST (index_node_list) {
	Tester::getIndexNodeList();
}END_TEST


void Tester::testDroppedList() {
	int err;
	auto_ptr<LogicalID> lid(new DBLogicalID(5));
	auto_ptr<IndexHandler> ih(new IndexHandler("d", "d", "d", lid.get()));
	auto_ptr<BTree> t(new BTree(new IntComparator(), ih.get()));
	ih->tree = t.get();
	err = t->createRoot();
	test(err, "createRoot");
	
	
	fail_if(IndexHandler::droppedNode != NEW_NODE);
	fail_unless(IndexHandler::droppedCachedNodes.empty());

	vector<nodeAddress_t> list, list2;
	list.insert(list.begin(), ih->firstNode);
	
	for (unsigned int i = 0; i < 2 * MAX_CACHED_NODES; i++) {
		Tester::getNode(list, NEW_NODE, ih.get());
	}
	
	nodeAddress_t toDel = list.at(0);
	fail_if(IndexHandler::droppedLastNode != NEW_NODE);
	fail_if(IndexHandler::droppedNode != NEW_NODE);
	for (unsigned int i = list.size(); i != 0; i--) {
		list2.insert(list2.begin(), list.at(0));
		deleteNode(list, ih.get(), 0);
		fail_if(IndexHandler::droppedNode != list2.at(0));
	}
	
	fail_if(IndexHandler::droppedLastNode != toDel);
	
	fail_unless(list.empty());
	fail_if(ih->firstNode != NEW_NODE);
	fail_if(ih->lastNode != NEW_NODE);
	
	fail_if(IndexHandler::droppedNode != list2.at(0));
	
	nodeAddress_t id, dropped;
	int count = list2.size();
	for(int i = 0; i < count; i++) {
		id = list2.at(i);
		dropped = IndexHandler::droppedNode;
		fail_if((dropped != id) && (i != count -1), "droppedNode = %d, list2.at = %d, i = %d, count = %d", dropped, id, i, count);
		Tester::getNode(list, NEW_NODE, ih.get());
		fail_if(list.at(0) != list2.at(i));
	}
	fail_if(IndexHandler::droppedNode != NEW_NODE);
	fail_if(IndexHandler::droppedLastNode != NEW_NODE, "droppedLast = %d", IndexHandler::droppedLastNode);
	
	t.release();
}

START_TEST (drop_node_list) {
	Tester::testDroppedList();
}END_TEST


void Tester::testDropIndex() {
	int err;
	auto_ptr<LogicalID> lid(new DBLogicalID(5));
	auto_ptr<IndexHandler> ih(new IndexHandler("d", "d", "d", lid.get()));
	auto_ptr<BTree> t(new BTree(new IntComparator(), ih.get()));
	ih->tree = t.get();
	err = t->createRoot();
	test(err, "createRoot");
	
	/*
	set<CachedNode*>::iterator it;
	it = ih->freeNodes.begin()->second.begin(); 
	for (;it != ih->freeNodes.begin()->second.end(); it++) {
		cout << *it << " ";
	}
	cout << endl;
	*/
	
	vector<nodeAddress_t> list, removed;
	list.insert(list.begin(), ih->firstNode);
	
	for (unsigned int i = 0; i < 4 * MAX_CACHED_NODES; i++) {
		getNode(list, NEW_NODE, ih.get());
	
		/*
		bool find = false;
		it = ih->freeNodes.begin()->second.begin(); 
			for (;it != ih->freeNodes.begin()->second.end(); it++) {
				cout << i << ": " << *it << " ";
				if (*it == NULL) {
					find = true;
				}
			}
			cout << endl << find << endl;;*/
	}
	//list zawiera wszystkie wezly indeksu
	
	nodeAddress_t id;
	int count = list.size() - 1;
	int pos;
	for(int i = count /2; i > 0; i--) {
		pos = rand() % list.size();
		id = list.at(pos);
		removed.insert(removed.begin(), id);
		deleteNode(list, ih.get(), pos);
	}
	//removed zawiera liste usunietych
	
	ih->drop();
	
	for (unsigned int i = 0; i < list.size(); i++) {
		removed.push_back(list.at(i));
	}
	//removed zawiera wszystkie poprzednio usuniete i cala liste indeksu
	list.clear();
	
	ih.reset(new IndexHandler("e", "e", "e", lid.get()));
	t.release();
	t.reset(new BTree(new IntComparator(), ih.get()));
	ih->tree = t.get();
	for (unsigned int i = 0; i < removed.size(); i++) {
		getNode(list, NEW_NODE, ih.get());
	}
	
	fail_if(IndexHandler::droppedNode != NEW_NODE);
	fail_if(IndexHandler::droppedLastNode != NEW_NODE);

	//list powinno zawierac odwrocona liste removed
	fail_if(list.size() != removed.size());
	
	for (unsigned int i = 0; i < list.size(); i++) {
		fail_if(list.at(i) != removed.at(list.size() - i -1), "na pozycji list %d, (removed: %d) list: %d, removed: %d. rozmiar listy: %d", i, list.size() - i -1, list.at(i), removed.at(list.size() - i -1), list.size());
	}
	t.release();
}

START_TEST (drop_index) {
	Tester::testDropIndex();
}END_TEST

/** template

START_TEST (name) {
}END_TEST

*/

Suite * dropping() {
	Suite *s = suite_create ("DropNodes");
	
	TCase *tc_core = tcase_create ("Drop");
	tcase_set_timeout(tc_core, 5); //timeout = 1s
	tcase_add_checked_fixture(tc_core, setup_listener, teardown_listener);
	tcase_add_test (tc_core, index_node_list);
	tcase_add_test (tc_core, drop_node_list);
	tcase_add_test (tc_core, drop_index);
	
    suite_add_tcase (s, tc_core);
	
	return s;
}
