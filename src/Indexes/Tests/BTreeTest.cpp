#include <Indexes/IndexHandler.h>
#include <Indexes/BTree.h>

#include <Suits.h>
#include <unistd.h>

using namespace Errors;
using namespace Indexes;

/** template

START_TEST (name) {
}END_TEST

*/

bool noSplit(BTree* t, Comparator* c, IndexHandler* ih) {
	CachedNode* croot;
	Node* root;
	
	nodeAddress_t rootAddr = Tester::getRoot(t);
	int err = ih->getNode(croot, rootAddr);
	test(err,"getNode()");
	
	croot->getNode(root, Tester::getPriority(t), NODE_READ);
	
	bool result = c->canTakeExtraKey(root);
	
	err = ih->freeNode(croot);
	test(err, "freeNode");
	return result;
}

void Tester::testRootDrop() {
	int err = 0;
	auto_ptr<LogicalID> dblid(new DBLogicalID(5));
	auto_ptr<IndexHandler> ih(new IndexHandler("d", "d", "d", dblid.get()));
	BTree *t = new BTree(new StringComparator(), ih.get());
	ih->tree = t;
	auto_ptr<Comparator> c(t->getNewComparator());
	
	err = t->createRoot();
	test(err, "createRoot");
	
	int rootLevel = t->rootLevel;
	nodeAddress_t adr1 = t->rootAddr;
	
	VisibilityResolver* v = IndexManager::getHandle()->resolver.get();
		
	lid_t lid = 3;
	ts_t tstamp = 120;
	tid_t tid = 110;
	v->begin(tid);
	RootEntry e(tstamp++, STORE_IXR_NULLVALUE, tid, lid++);
	c->setEntry(&e);
	
	while (noSplit(t, c.get(), ih.get())) {
		e.logicalID++;
		err = c->setValue(6);
		test(err, "ustawienie klucza");
		
		err = t->addEntry(c.get());
		test(err, "addEntry");
	}
	
	fail_if(rootLevel != t->rootPriority());
	fail_if(adr1 != t->rootAddr);
	
	e.logicalID++;
	err = c->setValue(6);
	test(err, "ustawienie klucza");
	
	err = t->addEntry(c.get());
	test(err, "addEntry");
	
	nodeAddress_t adr2 = t->rootAddr;
	fail_if (adr1 == adr2);
	fail_if (rootLevel == t->rootPriority());
	
	v->abort(tid);
	
	e.logicalID = 3;
	e.modyfierTransactionId = tid+tstamp+1000;
	v->begin(e.modyfierTransactionId);

	///////////////
	//wyczyszczenie lewego dziecka
	e.logicalID++;
	err = c->setValue(6);
	test(err, "ustawienie klucza");
	
	err = t->addEntry(c.get());
	test(err, "addEntry");
	
	//wyczyszczenie prawego dziecka
	e.logicalID++;
	err = c->setValue(7);
	test(err, "ustawienie klucza");
	
	err = t->addEntry(c.get());
	test(err, "addEntry");
	
	//scalenie dzieci
	e.logicalID++;
	err = c->setValue(6);
	test(err, "ustawienie klucza");
	
	err = t->addEntry(c.get());
	test(err, "addEntry");
	
	//wymiana roota
	e.logicalID++;
	err = c->setValue(6);
	test(err, "ustawienie klucza");
	
	err = t->addEntry(c.get());
	test(err, "addEntry");
	
	fail_if(adr2 == t->rootAddr);
	fail_if(rootLevel != t->rootPriority());
}

START_TEST (root_drop) {
	Tester::testRootDrop();
}END_TEST

START_TEST (init) {

fail_if(Tester::getUsedCount() != 0, "inicjalizacja drzewa -> zla liczba wolnych wezlow");

}END_TEST

void Tester::testCreateInt() {
	int err;
	auto_ptr<LogicalID> lid(new DBLogicalID(5));
	auto_ptr<IndexHandler> ih(new IndexHandler("d", "d", "d", lid.get()));
	BTree* t = new BTree(new IntComparator(), ih.get());
	ih->tree = t;
	
	err = t->createRoot();
	test(err, "createRoot");
	fail_if(Tester::getUsedCount() != 0, "tworzenie roota nie zwalnia wezlow");
	
	Comparator* c = t->getNewComparator();
	auto_ptr<Comparator> cc(c);
	tid_t tid = 15;
	IndexManager::getHandle()->begin(tid);
	RootEntry e(1,2,tid,4);
	err = c->setValue(2);
	test(err, "setValue(int)");
	
	c->setEntry(&e);
	
	Node* node;
	CachedNode* cnode;
	err = ih->getNode(cnode, NEW_NODE);
	test(err, "getNode(new)");
	fail_if(Tester::getUsedCount() != 1, "pobranie wezla zle ustawia liczbe uzywanych wezlow");
	
	cnode->getNode(node, 1, NODE_WRITE);
	
	auto_ptr<LogicalID> lid2(new DBLogicalID(5));
	auto_ptr<IndexHandler> ih2(new IndexHandler("d", "d", "d", lid2.get()));
	mark_point();
	BTree* t2 = new BTree(new IntComparator(), ih2.get());
	ih2->tree = t2;
	mark_point();
	node->leaf = true;
	
	nodeSize_t left1 = node->left;
	fail_if(left1 != MAX_NODE_CAPACITY, "zly rozmiar wolnej przestrzeni w wezle");
	
	mark_point();
	err = Tester::addEntry(t2, cnode, c);
	test(err, "addEntry(node, comp)");
	fail_if(Tester::getUsedCount() != 0, "wstawianie do drzewa nie zwalnia wezlow");
	fail_if (node->left >= left1, "wstawienie do wezla nie zmniejsza ilosci pozostalej przestrzeni");
	
	err = t->addEntry(c);
	test(err, "addEntry");
	
	fail_if(Tester::getUsedCount() != 0, "wstawianie do drzewa nie zwalnia wezlow");	
}

START_TEST (create_int) {
	Tester::testCreateInt();
}END_TEST

void Tester::testSplit() {
	int err;
		
	err = Tester::getCache()->count(90);
	if (err) (*(char*)0)++;
	
	auto_ptr<LogicalID> lid(new DBLogicalID(5));
	auto_ptr<IndexHandler> ih(new IndexHandler("d", "d", "d", lid.get()));
	BTree* t = new BTree(new IntComparator(), ih.get());
	ih->tree = t;
	err = t->createRoot();
	test(err, "createRoot");
	
	err = Tester::getCache()->count(90);
	if (err) (*(char*)0)++;
	tid_t tid = 15;
	IndexManager::getHandle()->begin(tid);
	RootEntry e(11,-2,tid,14);
	
	auto_ptr<Comparator> c(t->getNewComparator());
	mark_point();
	c->setEntry(&e);
	c->setValue(53);
	mark_point();
	while (noSplit(t, c.get(), ih.get())) {
		e.logicalID++;
		mark_point();
		err = t->addEntry(c.get());
		test(err, "addEntry");
		
		err = Tester::getCache()->count(90);
		if (err) (*(char*)0)++;
	}
	mark_point();
	err = noSplit(t, c.get(), ih.get());
	fail_if(err, "korzen nadal moze przyjac kolejny wpis");
	
	e.logicalID++;
	
	err = Tester::getCache()->count(90);
	if (err) (*(char*)0)++;
	err = t->addEntry(c.get());
	test(err, "addEntry");
		
	err = Tester::getCache()->count(90);
	//if (err) (*(char*)0)++;
	
	e.logicalID++;
	err = t->addEntry(c.get());
	test(err, "addEntry");
}

START_TEST (split) {
	Tester::testSplit();
}END_TEST

void Tester::testAdding() {
	int err;
	auto_ptr<LogicalID> lid(new DBLogicalID(5));
	auto_ptr<IndexHandler> ih(new IndexHandler("d", "d", "d", lid.get()));
	BTree* t = new BTree(new IntComparator(), ih.get());
	ih->tree = t;
	
	err = t->createRoot();
	test(err, "createRoot");
	auto_ptr<Comparator> c (t->getNewComparator());
	tid_t tid = 15;
	IndexManager::getHandle()->begin(tid);
	RootEntry e(11,12,tid,14);
	err = c->setValue(53);
	c->setEntry(&e);
	
	fail_if(err, "setValue");
	
	for(int i = 0; i < 2000; i++) {
		e.logicalID++;
		mark_point();
		err = t->addEntry(c.get());
		test(err, "addEntry");
		err = Tester::getUsedCount();
		fail_if (err, "po dodaniu uzywanych wezlow jest %d", err);
	}
	
	e.logicalID++;
	err = t->addEntry(c.get());
	test(err, "addEntry");
	err = Tester::getUsedCount();
	fail_if (err, "po dodaniu uzywanych wezlow jest %d", err);
	
	for (int i = 0; i < 7000; i++) {
		int j=4+(rand() % 1000); //<4;1003>
		c->setValue(j);
		mark_point();
		e.logicalID++;
		err = t->addEntry(c.get());
		test(err, "addEntry");
		err = Tester::getUsedCount();
		fail_if (err, "po dodaniu uzywanych wezlow jest %d", err);
	}
	
	int j=4+(rand() % 1000); //<4;1003>
		c->setValue(j);
		mark_point();
		e.logicalID++;
		err = t->addEntry(c.get());
		test(err, "addEntry");
		err = Tester::getUsedCount();
		fail_if (err, "po dodaniu uzywanych wezlow jest %d", err);
	
}

START_TEST (adding) {
	Tester::testAdding();
}END_TEST

void Tester::testSearching() {
	int err;
	auto_ptr<LogicalID> lidh(new DBLogicalID(5));
	auto_ptr<IndexHandler> ih(new IndexHandler("d", "d", "d", lidh.get()));
	BTree* t = new BTree(new IntComparator(), ih.get());
	ih->tree = t;
	
	err = t->createRoot();
	test(err, "createRoot");
	Comparator* c = t->getNewComparator();
	tid_t tid = 15;
	IndexManager::getHandle()->begin(tid);
	RootEntry e(1,2,tid,4);
	lid_t lid = 14;
	bool added = false;
	int i;
	for (i = 0; i < 50000; i++) {//1184 2185; 820 - 821
		int j=4+(rand() % 1000); //<4;1003>
		RootEntry additionalEntry(j%19,j%17,tid,j);
		additionalEntry.logicalID = lid++;
		c->setEntry(&additionalEntry);
		j = j > 500 ? j : -j;
		err = c->setValue(j);
		test(err, "setValue");
		
		if (!added && (3 > (rand() % 1000))) {
			mark_point();
			err = c->setValue(2);
			c->setEntry(&e);
			//additionalEntry.logicalID = lid++;
			err = t->addEntry(c);
			test(err, "addEntry");
			added = true;
			c->setEntry(&additionalEntry);
		}
		mark_point();
		c->getEntry()->logicalID = lid++;
		mark_point();
		
		err = Tester::getCache()->count(180);
		err++;
		err = t->addEntry(c);
		test(err, "addEntry");
		mark_point();
	}
	mark_point();
	if (!added) {
		mark_point();
		err = c->setValue(2);
		//e.logicalID = lid++;
		c->setEntry(&e);
		err = t->addEntry(c);
		test(err, "addEntry");
		added = true;
	}
	mark_point();
	/* w drzewie jest duzo wartosci < 0;
	 * jedna wartosc 2
	 * duzo wartosci >= 3
	 */ 
	
	Comparator *search = t->getNewComparator();
	
	//szukanie rownego
	
	search->setValue(2);
	
	RootEntry* eResult = NULL;
	CachedNode* nResult = NULL;
	mark_point();
	err = t->find(search, nResult, eResult);
	test(err, "find");
	
	fail_if(nResult == NULL, "nie znaleziono wpisu %d", nResult);
	err = memcmp(eResult, &e, sizeof(RootEntry)); //1197472902
	fail_if (err, "znaleziono inny obiekt");
	
	fail_if(Tester::getUsedCount() != 1, "find zle zwalnia wezly: %d", Tester::getUsedCount());
	
	ih->freeNode(nResult);
	fail_if(Tester::getUsedCount() != 0, "zwolniono zly wezel");
	//szukanie takiego ktorego nie ma ale sa wieksze od niego
	
	search->setValue(1);
	
	eResult = NULL;
	nResult = NULL;
	
	err = t->find(search, nResult, eResult);
	test(err, "find");
	
	fail_if(Tester::getUsedCount() != 1, "find zle zwalnia wezly");
	
	fail_if(nResult == NULL, "nie znaleziono wpisu %d", nResult);
	err = memcmp(eResult, &e, sizeof(RootEntry));
	fail_if (err, "znaleziono inny obiekt");
	
	ih->freeNode(nResult);
	fail_if(Tester::getUsedCount() != 0, "zwolniono zly wezel");
	//szukanie wiekszego niz max
	
	err = search->setValue(2000);
	test(err, "setvalue");
	nResult = NULL;
	eResult = NULL;
	err = t->find(search, nResult, eResult);
	test(err, "find");
	fail_if(nResult, "find nie powinien byl nic znalezc");
	fail_if(Tester::getUsedCount() != 0, "find zle zwalnia wezly");
	fail_if(nResult || eResult, "find zle ustawia wynik");
	
}

START_TEST (searching) {
	Tester::testSearching();
}END_TEST

Suite * btree() {
	Suite *s = suite_create ("Btree");
	
	TCase *tc_core = tcase_create ("Btree_operation");
	tcase_set_timeout(tc_core, 22); //timeout = 1s
	tcase_add_checked_fixture(tc_core, setup_listener, teardown_listener);
	tcase_add_test (tc_core, init);
    tcase_add_test (tc_core, create_int);
    tcase_add_test (tc_core, split);
    tcase_add_test (tc_core, root_drop);
    
    // dlugie testy
    tcase_add_test (tc_core, adding);
    tcase_add_test (tc_core, searching);
   
   
   // tcase_add_test (tc_core, loop);
	//tcase_add_test_raise_signal(tc_core, segfault, SIGSEGV);
    suite_add_tcase (s, tc_core);
	
	return s;
}
