#include "Indexes/IndexHandler.h"
#include "Indexes/BTree.h"

#include "Suits.h"


using namespace Errors;
using namespace Indexes;

/** template

START_TEST (name) {
}END_TEST

*/

START_TEST (sieve) {
	int err;
	
	auto_ptr<TransactionID> tid(new TransactionID(5));
	tid->setTimeStamp(5);
	lid_t whatever = 3;
	EntrySieve sieve(tid.get(), whatever);
	
	// dodanie, nie usuniete, zatwierdzone, lid
	RootEntry e(1, STORE_IXR_NULLVALUE, STORE_IXR_NULLVALUE, 4);
	sieve.add(&e);
	
	sieveContent_t * result = sieve.getResult();
	
	err = result->size();
	
	fail_if(err != 1, "sieve zle przesiewa");
	
}END_TEST

void Tester::testBadNode() {
	int err;

	auto_ptr<LogicalID> lid(new DBLogicalID(5));
	auto_ptr<IndexHandler> ih(new IndexHandler("d", "d", "d", lid.get()));
	BTree* tree = new BTree(new IntComparator(), ih.get());
	ih->tree = tree;
	err = tree->createRoot();
	test(err, "createRoot");
	
	auto_ptr<QueryNode> badnode(new ParamNode("name"));
	
	auto_ptr<TransactionID> tid(new TransactionID(1));
	EntrySieve sieve(tid.get(), ih->getId());
	
	RootEntry e(1,2,3,4);
	auto_ptr<Constraints>
					constr(new ExactConstraints(badnode.get()));
	mark_point();
	err = constr->search(tree, &sieve);
	test(err, "find");	
}

START_TEST (bad_node) {
	Tester::testBadNode();	
}END_TEST

void Tester::testExactInt() {
	int err;
	auto_ptr<LogicalID> lid(new DBLogicalID(5));
	auto_ptr<IndexHandler> ih(new IndexHandler("d", "d", "d", lid.get()));
	auto_ptr<BTree> tree(new BTree(new IntComparator(), ih.get()));
	ih->tree = tree.get();
	err = tree->createRoot();
	test(err, "createRoot");
	
	auto_ptr<QueryNode>
					inode(new IntNode(6)),
					snode(new StringNode("ala")),
					dnode(new DoubleNode(2.5))
			;

	auto_ptr<Constraints>
					constr(new ExactConstraints(inode.get()));
			;

	
	
	
	tid_t tid2 = 15;
	IndexManager::getHandle()->begin(tid2);
	
	RootEntry e(1, STORE_IXR_NULLVALUE, tid2, 4);
	
	auto_ptr<TransactionID> tid(new TransactionID(tid2));
	EntrySieve sieve(tid.get(), ih->getId());
	
	mark_point();
	err = constr->search(tree.get(), &sieve);
	test(err, "find");
	
	err = sieve.getResult()->size();
	fail_if(err, "zla ilosc przesianych elementow");
	
	auto_ptr<Comparator> c(tree->getNewComparator());
	
	c->setEntry(&e);
	err = c->setValue(6);
	test(err, "setValue");
	
	err = tree->addEntry(c.get());
	test(err, "addEntry");
	
	EntrySieve sieve2(tid.get(), ih->getId());
	mark_point();
	err = constr->search(tree.get(), &sieve2);
	mark_point();
	test(err, "search");
	
	sieveContent_t* vec = sieve2.getResult();
	err = vec->size();
	//err = sieve.getResult()->size();
	fail_if(err != 1, "zla ilosc przesianych elementow. elementow: %d", err);
	
	EntrySieve sieve3(tid.get(), ih->getId());
	constr.reset(new ExactConstraints(snode.get()));
	err = constr->search(tree.get(), &sieve3);
	fail_unless(err, "powinien byc blad a go nie bylo");
	
	EntrySieve sieve4(tid.get(), ih->getId());
	constr.reset(new ExactConstraints(snode.get()));
	err = constr->search(tree.get(), &sieve4);
	fail_unless(err, "powinien byc blad a go nie bylo");
	tree.release();
}

START_TEST (exact_int) {
	Tester::testExactInt();
}END_TEST

void add(BTree* t, Comparator* c, int j) {
		int err;
		mark_point();
		err = c->setValue(j);
		test(err, "setValue");
		
		Tester::getEntry(c)->logicalID++;
		
		err = t->addEntry(c);
		test(err, "addEntry");
}

void addf(BTree* t, Comparator* c, int j) {
		int err;
		mark_point();
		err = c->setValue(j+0.0);
		test(err, "setValue");
		
		Tester::getEntry(c)->logicalID++;
		
		err = t->addEntry(c);
		test(err, "addEntry");
}

int valueCount(BTree* t, Constraints* constr) {
	int err;
	auto_ptr<TransactionID> tid(new TransactionID(17)); //nie zmieniac wartosci 17
	EntrySieve sieve(tid.get(), t->ih->getId());
	err = constr->search(t, &sieve);
	test(err, "find");
	if (err) {
		char* ptr = NULL;
		ptr[0]++;
	}
	return sieve.getResult()->size();
}

int valueExactCount(BTree* t, int v) {
	auto_ptr<QueryNode>
					in(new IntNode(v));
					
	auto_ptr<Constraints> bc(new ExactConstraints(in.get()));
	return valueCount(t, bc.get());
}

int valueLCount(BTree* t, bool l, int v) {
	auto_ptr<QueryNode>
					in(new IntNode(v));
					
	auto_ptr<Constraints> bc(new LeftBoundedConstraints(l, in.get()));
	return valueCount(t, bc.get());
}

int valueRCount(BTree* t, int v, bool r) {
	auto_ptr<QueryNode>
					in(new IntNode(v));
					
	auto_ptr<Constraints> bc(new RightBoundedConstraints(in.get(), r));
	return valueCount(t, bc.get());
}

int value2Count(BTree* t, bool left, int l, int r, bool right) {
	auto_ptr<QueryNode>
					il(new IntNode(l)),
					ir(new IntNode(r));
					
	auto_ptr<Constraints> bc(new TwoSideConstraints(left, il.get(), ir.get(), right));
	return valueCount(t, bc.get());			
}

void Tester::testBigTree() {
	int err;
	auto_ptr<QueryNode>
					inode(new IntNode(2)),
					inodeFirst(new IntNode(-2));
	auto_ptr<Constraints>
					constr(new ExactConstraints(inode.get()));
					
	tid_t tid = 17; //wazne ze w valueCount tez jest 17
	IndexManager::getHandle()->begin(tid);
	RootEntry e(6, STORE_IXR_NULLVALUE, tid, 4);
	
	auto_ptr<LogicalID> lid(new DBLogicalID(5));
	auto_ptr<IndexHandler> ih(new IndexHandler("d", "d", "d", lid.get()));
	auto_ptr<BTree> t(new BTree(new IntComparator(), ih.get()));
	ih->tree = t.get();
	auto_ptr<Comparator> c(t->getNewComparator());
	
	err = t->createRoot();
	test(err, "createRoot");
	
	c->setEntry(&e);
	
	// (-2100; inf)
	err = valueLCount(t.get(), false, -2100);
	fail_if(err, "zla ilosc wpisow");
	
	// (-inf; -2100)
	err = valueRCount(t.get(), -2100, false);
	fail_if(err != 0, "zla ilosc wpisow");
	
	int count0 = 4000; //ile innych wezlow
	int count_2 = 100; // ile 2
	int count_m2 = 200; // ile -2
	int count_m1 = 300; // ile -1
	int count_1 = 150; //ile 1 
	
	int count_m2000 = 120;
	int count_m1900 = 130;
	int count_m1800 = 160;
	
	int count_2000 = 110;
	int count_1900 = 190;
	int count_1800 = 180;
	
	int all = count0 + count_2 + count_m2 + count_m1 + count_1 + count_m2000 +
				count_m1900 + count_m1800 + count_1800 + count_1900 + count_2000;
				
	for (int i = 0; i < count0; i++) {
		int j=4+(rand() % 1000); //<4;1003>
		j = j > 500 ? j : -j;
		//fail_if (j == 2, "wstawilem 2");
		add(t.get(), c.get(), j);
	}
	
	for (int i = 0; i < count_2; i++) {
		add(t.get(), c.get(), 2);
	}
	
	for (int i = 0; i < count_1; i++) {
		add(t.get(), c.get(), 1);
	}
	
	//exact 2
	err = valueCount(t.get(), constr.get());
	fail_if(err != count_2, "zla ilosc znalezionych wartosci, znaleziono: %d", err);
	
	fail_if (Tester::getUsedCount(), "zle zwalniane wezly");
	
	
	for (int i = 0; i < count_m2; i++) {
		add(t.get(), c.get(), -2);
	}
	
	for (int i = 0; i < count_m1; i++) {
		add(t.get(), c.get(), -1);
	}
	
	for (int i = 0; i < count_m2000; i++) {
		add(t.get(), c.get(), -2000);
	}
	
	for (int i = 0; i < count_m1900; i++) {
		add(t.get(), c.get(), -1900);
	}
	
	for (int i = 0; i < count_m1800; i++) {
		add(t.get(), c.get(), -1800);
	}
	
	for (int i = 0; i < count_1800; i++) {
		add(t.get(), c.get(), 1800);
	}
	
	for (int i = 0; i < count_1900; i++) {
		add(t.get(), c.get(), 1900);
	}
	
	for (int i = 0; i < count_2000; i++) {
		add(t.get(), c.get(), 2000);
	}
	
	//exact 2
	err = valueExactCount(t.get(), 2);
	fail_if(err != count_2, "zla ilosc wpisow");
	
	//exact 0
	err = valueExactCount(t.get(), 0);
	fail_if(err != 0, "zla ilosc wpisow. znaleziono: %d", err);
	
	//exact 2100
	err = valueExactCount(t.get(), 2100);
	fail_if(err != 0, "zla ilosc wpisow");
	
	//exact -2100
	err = valueExactCount(t.get(), -2100);
	fail_if(err != 0, "zla ilosc wpisow");
	
	//<-2; 2>
	err = value2Count(t.get(), true, -2, 2, true);
	fail_if(err != count_m2 + count_m1 + count_1 + count_2, "zla ilosc wpisow");
	
	// (-2; 2>
	err = value2Count(t.get(), false, -2, 2, true);
	fail_if(err != count_m1 + count_1 + count_2, "zla ilosc wpisow");
	
	// (-2; 2)
	err = value2Count(t.get(), false, -2, 2, false);
	fail_if(err != count_m1 + count_1, "zla ilosc wpisow");
	
	// (2; -12)
	err = value2Count(t.get(), false, 2, -12, false);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// (-2; -2)
	err = value2Count(t.get(), false, -2, -2, false);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// <-2100; -1900>
	err = value2Count(t.get(), true, -2100, -1900, true);
	fail_if(err != count_m2000 + count_m1900, "zla ilosc wpisow");
	
	// (-2100; -1900>
	err = value2Count(t.get(), false, -2100, -1900, true);
	fail_if(err != count_m2000 + count_m1900, "zla ilosc wpisow");
	
	// (-2100; -1800)
	err = value2Count(t.get(), false, -2100, -1800, false);
	fail_if(err != count_m2000 + count_m1900, "zla ilosc wpisow");
	
	// (-2100; -1600)
	err = value2Count(t.get(), false, -2100, -1600, false);
	fail_if(err != count_m2000 + count_m1900 + count_m1800, "zla ilosc wpisow");
	
	// <-2000; -1800)
	err = value2Count(t.get(), true, -2000, -1800, false);
	fail_if(err != count_m2000 + count_m1900, "zla ilosc wpisow");
	
	// <-2000; -1800)
	err = value2Count(t.get(), true, -2000, -1800, false);
	fail_if(err != count_m2000 + count_m1900, "zla ilosc wpisow");
	
	// (-2000; -1800)
	err = value2Count(t.get(), false, -2000, -1800, false);
	fail_if(err != count_m1900, "zla ilosc wpisow");
	
	// <1800; 2800>
	err = value2Count(t.get(), true, 1800, 2800, true);
	fail_if(err != count_1800 + count_1900 + count_2000, "zla ilosc wpisow");
	
	// <1800; 2800)
	err = value2Count(t.get(), true, 1800, 2800, false);
	fail_if(err != count_1800 + count_1900 + count_2000, "zla ilosc wpisow");
	
	// <1800; 2000>
	err = value2Count(t.get(), true, 1800, 2000, true);
	fail_if(err != count_1800 + count_1900 + count_2000, "zla ilosc wpisow");
	
	// <1800; 2000)
	err = value2Count(t.get(), true, 1800, 2000, false);
	fail_if(err != count_1800 + count_1900, "zla ilosc wpisow");
	
	// (1800; 2000)
	err = value2Count(t.get(), false, 1800, 2000, false);
	fail_if(err != count_1900, "zla ilosc wpisow");
	
	// (1800; 2000>
	err = value2Count(t.get(), false, 1800, 2000, true);
	fail_if(err != count_1900 + count_2000, "zla ilosc wpisow");
	
	// <2000; 2100)
	err = value2Count(t.get(), true, 2000, 2100, false);
	fail_if(err != count_2000, "zla ilosc wpisow");
	
	// (2000; 2100)
	err = value2Count(t.get(), false, 2000, 2100, false);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// <2100; 2200>
	err = value2Count(t.get(), true, 2100, 2200, true);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// (2100; 2200>
	err = value2Count(t.get(), false, 2100, 2200, true);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// <2100; 2200)
	err = value2Count(t.get(), true, 2100, 2200, false);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// (2100; 2200)
	err = value2Count(t.get(), false, 2100, 2200, false);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// <1900; inf)
	err = valueLCount(t.get(), true, 1900);
	fail_if(err != count_1900 + count_2000, "zla ilosc wpisow");
	
	// (1800; inf)
	err = valueLCount(t.get(), false, 1800);
	fail_if(err != count_1900 + count_2000, "zla ilosc wpisow");
	
	// <1900; inf)
	err = valueLCount(t.get(), true, 1900);
	fail_if(err != count_1900 + count_2000, "zla ilosc wpisow");
	
	// <1850; inf)
	err = valueLCount(t.get(), true, 1850);
	fail_if(err != count_1900 + count_2000, "zla ilosc wpisow");
	
	// (2000; inf)
	err = valueLCount(t.get(), false, 2000);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// <2100; inf)
	err = valueLCount(t.get(), true, 2100);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// (2100; inf)
	err = valueLCount(t.get(), false, 2100);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// <-2100; inf)
	err = valueLCount(t.get(), true, -2100);
	fail_if(err != all, "zla ilosc wpisow");
	
	// (-2100; inf)
	err = valueLCount(t.get(), false, -2100);
	fail_if(err != all, "zla ilosc wpisow");
	
	// (-inf; -2100)
	err = valueRCount(t.get(), -2100, false);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// (-inf; -2000)
	err = valueRCount(t.get(), -2000, false);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// (-inf; -2000>
	err = valueRCount(t.get(), -2000, true);
	fail_if(err != count_m2000, "zla ilosc wpisow");
	
	// (-inf; -1900)
	err = valueRCount(t.get(), -1900, false);
	fail_if(err != count_m2000, "zla ilosc wpisow");
	
	// (-inf; -1900>
	err = valueRCount(t.get(), -1900, true);
	fail_if(err != count_m2000 + count_m1900, "zla ilosc wpisow");
	
	// (-inf; -1850>
	err = valueRCount(t.get(), -1850, true);
	fail_if(err != count_m2000 + count_m1900, "zla ilosc wpisow");
	
	// (-inf; 2100>
	err = valueRCount(t.get(), 2100, true);
	fail_if(err != all, "zla ilosc wpisow");
	
	// (-inf; 2100)
	err = valueRCount(t.get(), 2100, false);
	fail_if(err != all, "zla ilosc wpisow");
	
	t.release();
	
}

START_TEST (big_tree) {
	Tester::testBigTree();
}END_TEST

void Tester::testBigTreeDouble() {
	int err;
	auto_ptr<QueryNode>
					inode(new DoubleNode(2.0)),
					inodeFirst(new DoubleNode(-2.0));
	auto_ptr<Constraints>
					constr(new ExactConstraints(inode.get()));
					
	tid_t tid = 17; //wazne ze w valueCount tez jest 17
	IndexManager::getHandle()->begin(tid);
	RootEntry e(6, STORE_IXR_NULLVALUE, tid, 4);
	
	auto_ptr<LogicalID> lid(new DBLogicalID(5));
	auto_ptr<IndexHandler> ih(new IndexHandler("d", "d", "d", lid.get()));
	auto_ptr<BTree> t(new BTree(new DoubleComparator(), ih.get()));
	ih->tree = t.get();
	auto_ptr<Comparator> c(t->getNewComparator());
	
	err = t->createRoot();
	test(err, "createRoot");
	
	c->setEntry(&e);
	int count0 = 4000; //ile innych wezlow
	int count_2 = 100; // ile 2
	int count_m2 = 200; // ile -2
	int count_m1 = 300; // ile -1
	int count_1 = 150; //ile 1 
	
	int count_m2000 = 120;
	int count_m1900 = 130;
	int count_m1800 = 160;
	
	int count_2000 = 110;
	int count_1900 = 190;
	int count_1800 = 180;
	
	int all = count0 + count_2 + count_m2 + count_m1 + count_1 + count_m2000 +
				count_m1900 + count_m1800 + count_1800 + count_1900 + count_2000;
				
	for (int i = 0; i < count0; i++) {
		int j=4+(rand() % 1000); //<4;1003>
		j = j > 500 ? j : -j;
		//fail_if (j == 2, "wstawilem 2");
		addf(t.get(), c.get(), j);
	}
	
	for (int i = 0; i < count_2; i++) {
		addf(t.get(), c.get(), 2);
	}
	
	for (int i = 0; i < count_1; i++) {
		addf(t.get(), c.get(), 1);
	}
	
	//exact 2
	err = valueCount(t.get(), constr.get());
	fail_if(err != count_2, "zla ilosc znalezionych wartosci, znaleziono: %d", err);
	
	fail_if (Tester::getUsedCount(), "zle zwalniane wezly");
	
	
	for (int i = 0; i < count_m2; i++) {
		addf(t.get(), c.get(), -2);
	}
	
	for (int i = 0; i < count_m1; i++) {
		addf(t.get(), c.get(), -1);
	}
	
	for (int i = 0; i < count_m2000; i++) {
		addf(t.get(), c.get(), -2000);
	}
	
	for (int i = 0; i < count_m1900; i++) {
		addf(t.get(), c.get(), -1900);
	}
	
	for (int i = 0; i < count_m1800; i++) {
		addf(t.get(), c.get(), -1800);
	}
	
	for (int i = 0; i < count_1800; i++) {
		addf(t.get(), c.get(), 1800);
	}
	
	for (int i = 0; i < count_1900; i++) {
		addf(t.get(), c.get(), 1900);
	}
	
	for (int i = 0; i < count_2000; i++) {
		addf(t.get(), c.get(), 2000);
	}
	
	//exact 2
	err = valueExactCount(t.get(), 2);
	fail_if(err != count_2, "zla ilosc wpisow");
	
	//exact 0
	err = valueExactCount(t.get(), 0);
	fail_if(err != 0, "zla ilosc wpisow. znaleziono: %d", err);
	
	//exact 2100
	err = valueExactCount(t.get(), 2100);
	fail_if(err != 0, "zla ilosc wpisow");
	
	//exact -2100
	err = valueExactCount(t.get(), -2100);
	fail_if(err != 0, "zla ilosc wpisow");
	
	//<-2; 2>
	err = value2Count(t.get(), true, -2, 2, true);
	fail_if(err != count_m2 + count_m1 + count_1 + count_2, "zla ilosc wpisow");
	
	// (-2; 2>
	err = value2Count(t.get(), false, -2, 2, true);
	fail_if(err != count_m1 + count_1 + count_2, "zla ilosc wpisow");
	
	// (-2; 2)
	err = value2Count(t.get(), false, -2, 2, false);
	fail_if(err != count_m1 + count_1, "zla ilosc wpisow");
	
	// (2; -12)
	err = value2Count(t.get(), false, 2, -12, false);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// (-2; -2)
	err = value2Count(t.get(), false, -2, -2, false);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// <-2100; -1900>
	err = value2Count(t.get(), true, -2100, -1900, true);
	fail_if(err != count_m2000 + count_m1900, "zla ilosc wpisow");
	
	// (-2100; -1900>
	err = value2Count(t.get(), false, -2100, -1900, true);
	fail_if(err != count_m2000 + count_m1900, "zla ilosc wpisow");
	
	// (-2100; -1800)
	err = value2Count(t.get(), false, -2100, -1800, false);
	fail_if(err != count_m2000 + count_m1900, "zla ilosc wpisow");
	
	// (-2100; -1600)
	err = value2Count(t.get(), false, -2100, -1600, false);
	fail_if(err != count_m2000 + count_m1900 + count_m1800, "zla ilosc wpisow");
	
	// <-2000; -1800)
	err = value2Count(t.get(), true, -2000, -1800, false);
	fail_if(err != count_m2000 + count_m1900, "zla ilosc wpisow");
	
	// <-2000; -1800)
	err = value2Count(t.get(), true, -2000, -1800, false);
	fail_if(err != count_m2000 + count_m1900, "zla ilosc wpisow");
	
	// (-2000; -1800)
	err = value2Count(t.get(), false, -2000, -1800, false);
	fail_if(err != count_m1900, "zla ilosc wpisow");
	
	// <1800; 2800>
	err = value2Count(t.get(), true, 1800, 2800, true);
	fail_if(err != count_1800 + count_1900 + count_2000, "zla ilosc wpisow");
	
	// <1800; 2800)
	err = value2Count(t.get(), true, 1800, 2800, false);
	fail_if(err != count_1800 + count_1900 + count_2000, "zla ilosc wpisow");
	
	// <1800; 2000>
	err = value2Count(t.get(), true, 1800, 2000, true);
	fail_if(err != count_1800 + count_1900 + count_2000, "zla ilosc wpisow");
	
	// <1800; 2000)
	err = value2Count(t.get(), true, 1800, 2000, false);
	fail_if(err != count_1800 + count_1900, "zla ilosc wpisow");
	
	// (1800; 2000)
	err = value2Count(t.get(), false, 1800, 2000, false);
	fail_if(err != count_1900, "zla ilosc wpisow");
	
	// (1800; 2000>
	err = value2Count(t.get(), false, 1800, 2000, true);
	fail_if(err != count_1900 + count_2000, "zla ilosc wpisow");
	
	// <2000; 2100)
	err = value2Count(t.get(), true, 2000, 2100, false);
	fail_if(err != count_2000, "zla ilosc wpisow");
	
	// (2000; 2100)
	err = value2Count(t.get(), false, 2000, 2100, false);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// <2100; 2200>
	err = value2Count(t.get(), true, 2100, 2200, true);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// (2100; 2200>
	err = value2Count(t.get(), false, 2100, 2200, true);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// <2100; 2200)
	err = value2Count(t.get(), true, 2100, 2200, false);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// (2100; 2200)
	err = value2Count(t.get(), false, 2100, 2200, false);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// <1900; inf)
	err = valueLCount(t.get(), true, 1900);
	fail_if(err != count_1900 + count_2000, "zla ilosc wpisow");
	
	// (1800; inf)
	err = valueLCount(t.get(), false, 1800);
	fail_if(err != count_1900 + count_2000, "zla ilosc wpisow");
	
	// <1900; inf)
	err = valueLCount(t.get(), true, 1900);
	fail_if(err != count_1900 + count_2000, "zla ilosc wpisow");
	
	// <1850; inf)
	err = valueLCount(t.get(), true, 1850);
	fail_if(err != count_1900 + count_2000, "zla ilosc wpisow");
	
	// (2000; inf)
	err = valueLCount(t.get(), false, 2000);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// <2100; inf)
	err = valueLCount(t.get(), true, 2100);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// (2100; inf)
	err = valueLCount(t.get(), false, 2100);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// <-2100; inf)
	err = valueLCount(t.get(), true, -2100);
	fail_if(err != all, "zla ilosc wpisow");
	
	// (-2100; inf)
	err = valueLCount(t.get(), false, -2100);
	fail_if(err != all, "zla ilosc wpisow");
	
	// (-inf; -2100)
	err = valueRCount(t.get(), -2100, false);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// (-inf; -2000)
	err = valueRCount(t.get(), -2000, false);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// (-inf; -2000>
	err = valueRCount(t.get(), -2000, true);
	fail_if(err != count_m2000, "zla ilosc wpisow");
	
	// (-inf; -1900)
	err = valueRCount(t.get(), -1900, false);
	fail_if(err != count_m2000, "zla ilosc wpisow");
	
	// (-inf; -1900>
	err = valueRCount(t.get(), -1900, true);
	fail_if(err != count_m2000 + count_m1900, "zla ilosc wpisow");
	
	// (-inf; -1850>
	err = valueRCount(t.get(), -1850, true);
	fail_if(err != count_m2000 + count_m1900, "zla ilosc wpisow");
	
	// (-inf; 2100>
	err = valueRCount(t.get(), 2100, true);
	fail_if(err != all, "zla ilosc wpisow");
	
	// (-inf; 2100)
	err = valueRCount(t.get(), 2100, false);
	fail_if(err != all, "zla ilosc wpisow");
	
	t.release();
	
}


START_TEST (big_tree_double) {
	Tester::testBigTreeDouble();
}END_TEST

void Tester::testBigTreeString() {
	int err;
	auto_ptr<QueryNode>
					inode(new IntNode(9)),
					inodeFirst(new IntNode(6));
	auto_ptr<Constraints>
					constr(new ExactConstraints(inode.get()));
					
	tid_t tid = 17; //wazne ze w valueCount tez jest 17
	IndexManager::getHandle()->begin(tid);
	RootEntry e(6, STORE_IXR_NULLVALUE, tid, 4);
	
	auto_ptr<LogicalID> lid(new DBLogicalID(5));
	auto_ptr<IndexHandler> ih(new IndexHandler("d", "d", "d", lid.get()));
	auto_ptr<BTree> t(new BTree(new StringComparator(), ih.get()));
	ih->tree = t.get();
	auto_ptr<Comparator> c(t->getNewComparator());
	
	err = t->createRoot();
	test(err, "createRoot");
	
	c->setEntry(&e);
	
	//exact 9
	err = valueExactCount(t.get(), 9);
	fail_if(err, "zla ilosc wpisow");
	
	//<6; 9>
	err = value2Count(t.get(), true, 6, 9, true);
	fail_if(err, "zla ilosc wpisow");
	
	// (6; 9>
	err = value2Count(t.get(), false, 6, 9, true);
	fail_if(err, "zla ilosc wpisow");
	
	// (6; 9)
	err = value2Count(t.get(), false, 6, 9, false);
	fail_if(err, "zla ilosc wpisow");
	
	// (9; 6)
	err = value2Count(t.get(), false, 9, 6, false);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// (6; 6)
	err = value2Count(t.get(), false, 6, 6, false);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// <2; 4>
	err = value2Count(t.get(), true, 2, 4, true);
	fail_if(err, "zla ilosc wpisow: %d", err);
	
	// (2; 4>
	err = value2Count(t.get(), false, 2, 4, true);
	fail_if(err, "zla ilosc wpisow");
	
	// (2; 5)
	err = value2Count(t.get(), false, 2, 5, false);
	fail_if(err, "zla ilosc wpisow");
	
	// (2; 50)
	err = value2Count(t.get(), false, 2, 50, false);
	fail_if(err, "zla ilosc wpisow");
	
	// <3; 5)
	err = value2Count(t.get(), true, 3, 5, false);
	fail_if(err, "zla ilosc wpisow");
	
	// (3; 5)
	err = value2Count(t.get(), false, 3, 5, false);
	fail_if(err, "zla ilosc wpisow");
	
	// <91; 94>
	err = value2Count(t.get(), true, 91, 94, true);
	fail_if(err, "zla ilosc wpisow");
	
	// <91; 94)
	err = value2Count(t.get(), true, 91, 94, false);
	fail_if(err, "zla ilosc wpisow");
	
	// <91; 93>
	err = value2Count(t.get(), true, 91, 93, true);
	fail_if(err, "zla ilosc wpisow");
	
	// <91; 93)
	err = value2Count(t.get(), true, 91, 93, false);
	fail_if(err, "zla ilosc wpisow");
	
	// (91; 93)
	err = value2Count(t.get(), false, 91, 93, false);
	fail_if(err, "zla ilosc wpisow");
	
	// (91; 93>
	err = value2Count(t.get(), false, 91, 93, true);
	fail_if(err, "zla ilosc wpisow");
	
	// <93; 94)
	err = value2Count(t.get(), true, 93, 94, false);
	fail_if(err, "zla ilosc wpisow");
	
	// (93; 94)
	err = value2Count(t.get(), false, 93, 94, false);
	fail_if(err, "zla ilosc wpisow");
	
	// <94; 95>
	err = value2Count(t.get(), true, 94, 95, true);
	fail_if(err, "zla ilosc wpisow");
	
	// (94; 95>
	err = value2Count(t.get(), false, 94, 95, true);
	fail_if(err, "zla ilosc wpisow");
	
	// <94; 95)
	err = value2Count(t.get(), true, 94, 95, false);
	fail_if(err, "zla ilosc wpisow");
	
	// (94; 95)
	err = value2Count(t.get(), false, 94, 95, false);
	fail_if(err, "zla ilosc wpisow");
	
	// <92; inf)
	err = valueLCount(t.get(), true, 92);
	fail_if(err, "zla ilosc wpisow");
	
	// (91; inf)
	err = valueLCount(t.get(), false, 91);
	fail_if(err, "zla ilosc wpisow");
	
	// <92; inf)
	err = valueLCount(t.get(), true, 92);
	fail_if(err, "zla ilosc wpisow");
	
	// <915; inf)
	err = valueLCount(t.get(), true, 915);
	fail_if(err, "zla ilosc wpisow");
	
	// (93; inf)
	err = valueLCount(t.get(), false, 93);
	fail_if(err, "zla ilosc wpisow");
	
	// <94; inf)
	err = valueLCount(t.get(), true, 94);
	fail_if(err, "zla ilosc wpisow");
	
	// (94; inf)
	err = valueLCount(t.get(), false, 94);
	fail_if(err, "zla ilosc wpisow");
	
	// <2; inf)
	err = valueLCount(t.get(), true, 2);
	fail_if(err, "zla ilosc wpisow");
	
	// (2; inf)
	err = valueLCount(t.get(), false, 2);
	fail_if(err, "zla ilosc wpisow");
	
	// (-inf; 2)
	err = valueRCount(t.get(), 2, false);
	fail_if(err, "zla ilosc wpisow");
	
	// (-inf; 3)
	err = valueRCount(t.get(), 3, false);
	fail_if(err, "zla ilosc wpisow");
	
	// (-inf; 3>
	err = valueRCount(t.get(), 3, true);
	fail_if(err, "zla ilosc wpisow");
	
	// (-inf; 4)
	err = valueRCount(t.get(), 4, false);
	fail_if(err, "zla ilosc wpisow");
	
	// (-inf; 4>
	err = valueRCount(t.get(), 4, true);
	fail_if(err, "zla ilosc wpisow");
	
	// (-inf; 45>
	err = valueRCount(t.get(), 45, true);
	fail_if(err, "zla ilosc wpisow");
	
	// (-inf; 94>
	err = valueRCount(t.get(), 94, true);
	fail_if(err, "zla ilosc wpisow");
	
	// (-inf; 94)
	err = valueRCount(t.get(), 94, false);
	fail_if(err, "zla ilosc wpisow");
	
	
	int count0 = 4000; //ile innych wezlow
	int count_9 = 100; // ile 9
	int count_6 = 200; // ile 6
	int count_7 = 300; // ile 7
	int count_8 = 150; //ile 8 
	
	
	int count_3 = 120;
	int count_4 = 130;
	int count_5 = 160;
	
	int count_93 = 110;
	int count_92 = 190;
	int count_91 = 180;
	
	int all = count0 + count_3 + count_4 + count_5 + count_6 + count_7 +
				count_8 + count_9 + count_91 + count_92 + count_93;
				
	for (int i = 0; i < count0; i++) {
		int j=4+(rand() % 1000); //<4;1003>
		j = j > 500 ? 900000 + j : 500000 + j;
		//fail_if (j == 2, "wstawilem 2");
		add(t.get(), c.get(), j);
	}
	
	for (int i = 0; i < count_9; i++) {
		add(t.get(), c.get(), 9);
	}
	
	for (int i = 0; i < count_8; i++) {
		add(t.get(), c.get(), 8);
	}
	
	//exact 2
	err = valueCount(t.get(), constr.get());
	fail_if(err != count_9, "zla ilosc znalezionych wartosci, znaleziono: %d", err);
	
	fail_if (Tester::getUsedCount(), "zle zwalniane wezly");
	
	
	for (int i = 0; i < count_6; i++) {
		add(t.get(), c.get(), 6);
	}
	
	for (int i = 0; i < count_7; i++) {
		add(t.get(), c.get(), 7);
	}
	
	for (int i = 0; i < count_3; i++) {
		add(t.get(), c.get(), 3);
	}
	
	for (int i = 0; i < count_4; i++) {
		add(t.get(), c.get(), 4);
	}
	
	for (int i = 0; i < count_5; i++) {
		add(t.get(), c.get(), 5);
	}
	
	for (int i = 0; i < count_91; i++) {
		add(t.get(), c.get(), 91);
	}
	
	for (int i = 0; i < count_92; i++) {
		add(t.get(), c.get(), 92);
	}
	
	for (int i = 0; i < count_93; i++) {
		add(t.get(), c.get(), 93);
	}
	
	//exact 9
	err = valueExactCount(t.get(), 9);
	fail_if(err != count_9, "zla ilosc wpisow");
	
	//exact 71
	err = valueExactCount(t.get(), 71);
	fail_if(err != 0, "zla ilosc wpisow. znaleziono: %d", err);
	
	//exact 94
	err = valueExactCount(t.get(), 94);
	fail_if(err != 0, "zla ilosc wpisow. powinno byc 0 a jest: %d", err);
	
	//exact 2
	err = valueExactCount(t.get(), 2);
	fail_if(err != 0, "zla ilosc wpisow");
	
	//<6; 9>
	err = value2Count(t.get(), true, 6, 9, true);
	fail_if(err != count_6 + count_7 + count_8 + count_9, "zla ilosc wpisow");
	
	// (6; 9>
	err = value2Count(t.get(), false, 6, 9, true);
	fail_if(err != count_7 + count_8 + count_9, "zla ilosc wpisow");
	
	// (6; 9)
	err = value2Count(t.get(), false, 6, 9, false);
	fail_if(err != count_7 + count_8, "zla ilosc wpisow");
	
	// (9; 6)
	err = value2Count(t.get(), false, 9, 6, false);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// (6; 6)
	err = value2Count(t.get(), false, 6, 6, false);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// <2; 4>
	err = value2Count(t.get(), true, 2, 4, true);
	fail_if(err != count_3 + count_4, "zla ilosc wpisow: %d", err);
	
	// (2; 4>
	err = value2Count(t.get(), false, 2, 4, true);
	fail_if(err != count_3 + count_4, "zla ilosc wpisow");
	
	// (2; 5)
	err = value2Count(t.get(), false, 2, 5, false);
	fail_if(err != count_3 + count_4, "zla ilosc wpisow");
	
	// (2; 50)
	err = value2Count(t.get(), false, 2, 50, false);
	fail_if(err != count_3 + count_4 + count_5, "zla ilosc wpisow");
	
	// <3; 5)
	err = value2Count(t.get(), true, 3, 5, false);
	fail_if(err != count_3 + count_4, "zla ilosc wpisow");
	
	// (3; 5)
	err = value2Count(t.get(), false, 3, 5, false);
	fail_if(err != count_4, "zla ilosc wpisow");
	
	// <91; 94>
	err = value2Count(t.get(), true, 91, 94, true);
	fail_if(err != count_91 + count_92 + count_93, "zla ilosc wpisow");
	
	// <91; 94)
	err = value2Count(t.get(), true, 91, 94, false);
	fail_if(err != count_91 + count_92 + count_93, "zla ilosc wpisow");
	
	// <91; 93>
	err = value2Count(t.get(), true, 91, 93, true);
	fail_if(err != count_91 + count_92 + count_93, "zla ilosc wpisow");
	
	// <91; 93)
	err = value2Count(t.get(), true, 91, 93, false);
	fail_if(err != count_91 + count_92, "zla ilosc wpisow");
	
	// (91; 93)
	err = value2Count(t.get(), false, 91, 93, false);
	fail_if(err != count_92, "zla ilosc wpisow");
	
	// (91; 93>
	err = value2Count(t.get(), false, 91, 93, true);
	fail_if(err != count_92 + count_93, "zla ilosc wpisow");
	
	// <93; 94)
	err = value2Count(t.get(), true, 93, 94, false);
	fail_if(err != count_93, "zla ilosc wpisow");
	
	// (93; 94)
	err = value2Count(t.get(), false, 93, 94, false);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// <94; 95>
	err = value2Count(t.get(), true, 94, 95, true);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// (94; 95>
	err = value2Count(t.get(), false, 94, 95, true);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// <94; 95)
	err = value2Count(t.get(), true, 94, 95, false);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// (94; 95)
	err = value2Count(t.get(), false, 94, 95, false);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// <92; inf)
	err = valueLCount(t.get(), true, 92);
	fail_if(err != count_92 + count_93, "zla ilosc wpisow");
	
	// (91; inf)
	err = valueLCount(t.get(), false, 91);
	fail_if(err != count_92 + count_93, "zla ilosc wpisow");
	
	// <92; inf)
	err = valueLCount(t.get(), true, 92);
	fail_if(err != count_92 + count_93, "zla ilosc wpisow");
	
	// <915; inf)
	err = valueLCount(t.get(), true, 915);
	fail_if(err != count_92 + count_93, "zla ilosc wpisow");
	
	// (93; inf)
	err = valueLCount(t.get(), false, 93);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// <94; inf)
	err = valueLCount(t.get(), true, 94);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// (94; inf)
	err = valueLCount(t.get(), false, 94);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// <2; inf)
	err = valueLCount(t.get(), true, 2);
	fail_if(err != all, "zla ilosc wpisow");
	
	// (2; inf)
	err = valueLCount(t.get(), false, 2);
	fail_if(err != all, "zla ilosc wpisow");
	
	// (-inf; 2)
	err = valueRCount(t.get(), 2, false);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// (-inf; 3)
	err = valueRCount(t.get(), 3, false);
	fail_if(err != 0, "zla ilosc wpisow");
	
	// (-inf; 3>
	err = valueRCount(t.get(), 3, true);
	fail_if(err != count_3, "zla ilosc wpisow");
	
	// (-inf; 4)
	err = valueRCount(t.get(), 4, false);
	fail_if(err != count_3, "zla ilosc wpisow");
	
	// (-inf; 4>
	err = valueRCount(t.get(), 4, true);
	fail_if(err != count_3 + count_4, "zla ilosc wpisow");
	
	// (-inf; 45>
	err = valueRCount(t.get(), 45, true);
	fail_if(err != count_3 + count_4, "zla ilosc wpisow");
	
	// (-inf; 94>
	err = valueRCount(t.get(), 94, true);
	fail_if(err != all, "zla ilosc wpisow");
	
	// (-inf; 94)
	err = valueRCount(t.get(), 94, false);
	fail_if(err != all, "zla ilosc wpisow");
	
	int count_empty = 350;
	
	for (int i = 0; i < count_empty; i++) {
		err = c->setValue("");
		test(err, "setValue");
		
		Tester::getEntry(c.get())->logicalID++;
		
		err = t->addEntry(c.get());
		test(err, "addEntry");
	}
	// (-inf; 94)
	err = valueRCount(t.get(), 94, false);
	fail_if(err != all + count_empty, "zla ilosc wpisow");
	
	// (-inf; 2)
	err = valueRCount(t.get(), 2, false);
	fail_if(err != count_empty, "zla ilosc wpisow");
	
	t.release();
	
}

START_TEST (big_tree_string) {
	Tester::testBigTreeString();
}END_TEST

Suite * traveler() {
	Suite *s = suite_create ("Traveler");
	
	TCase *tc_sieve = tcase_create ("SieveTest");
	tcase_add_test (tc_sieve, sieve);
	
	TCase *tc_core = tcase_create ("ScopeSelection");
	tcase_set_timeout(tc_core, 4); //timeout = 1s
	tcase_add_checked_fixture(tc_core, setup_listener, teardown_listener);
	tcase_add_test (tc_core, exact_int);
	tcase_add_test (tc_core, big_tree);
	tcase_add_test (tc_core, big_tree_string);
	tcase_add_test (tc_core, big_tree_double);
	
	if (!Tester::isDebugMode()) {
		tcase_add_test_raise_signal(tc_core, bad_node, ASSERT_SIG);
	}
	
	suite_add_tcase (s, tc_sieve);
    suite_add_tcase (s, tc_core);
	
	return s;
}
