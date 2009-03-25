#include <Test/Indexes/Suits.h>
#include <Indexes/Cleaner.h>
#include <unistd.h>

using namespace Errors;
using namespace Indexes;

void Tester::testRollback() {
	fail_if(Tester::getActive().size() != 0);
	int err;
	auto_ptr<LogicalID> lid(new DBLogicalID(5));
	auto_ptr<IndexHandler> ih(new IndexHandler("d", "d", "d", lid.get()));
	auto_ptr<BTree> t(new BTree(new IntComparator(), ih.get()));
	ih->tree = t.get();
	
	err = t->createRoot();
	test(err, "createRoot");
	fail_if(Tester::getUsedCount() != 0, "tworzenie roota nie zwalnia wezlow");

	auto_ptr<Comparator> c (t->getNewComparator());
	err = c->setValue(2);
	
	fail_if(Tester::getActive().size() != 0);
	tid_t tid = 16;
	IndexManager::getHandle()->begin(tid);
	
	fail_if(Tester::getActive().size() != 1);
	
	RootEntry e(250, STORE_IXR_NULLVALUE, tid, 4);
	c->setEntry(&e);
	
	err = t->addEntry(c.get());
	test(err, "addEntry");
	err = Tester::getActive().find(tid)->second->entryCounter.size();
	fail_if(err != 1, "entryCounter powinno byc 1 a jest %d", err);
	
	
	IndexManager::getHandle()->abort(tid);
	
	fail_if(Tester::getActive().size() != 0, "oczekiwano 0 a jest: %d", Tester::getActive().size());
	fail_if(Tester::getRolledBack().size() != 1, "powinna byc jedna wycofana a jest %d", Tester::getRolledBack().size());
	fail_if(Tester::getCommitTs().size() != 0);
	
	tid = 17;
	IndexManager::getHandle()->begin(tid);
	
	auto_ptr<QueryNode> qnode(new IntNode(2));
	auto_ptr<Constraints> constr(new ExactConstraints(qnode.get()));
	auto_ptr<TransactionID> transactionID(new TransactionID(tid));
	
	auto_ptr<EntrySieve> sieve(new EntrySieve(transactionID.get(), ih->getId()));
	err = constr->search(t.get(), sieve.get());
	test(err, "search");
	
	CachedNode* cnode;
	Node* node;
	t->ih->getNode(cnode, Tester::getRoot(t.get()));
	cnode->getNode(node);
	fail_unless(node->size() > 0);
	
	auto_ptr<Cleaner> cleaner(new Cleaner());
	cleaner->clean(cnode, t->getNewComparator(), ih.get());
	fail_unless(node->size() == 0, "aktualny rozmiar wezla to: %d, a powinno byc 0", node->size());
	
	fail_if(Tester::getActive().size() != 1);
	fail_if(Tester::getRolledBack().size() != 0);
	fail_if(Tester::getCommitTs().size() != 0);
		
	//dodac i usunac w tej samej transakcji
	e.modyfierTransactionId = tid;
	err = t->addEntry(c.get());
	test(err, "addEntry");
	fail_unless(node->size() > 0);
	
	e.delTime = e.addTime + 1;
	e.addTime = STORE_IXR_NULLVALUE;
	err = t->addEntry(c.get());
	test(err, "addEntry");
	fail_unless(node->size() == 0, "aktualny rozmiar wezla to: %d, a powinno byc 0", node->size());
	
	fail_if(Tester::getActive().size() != 1);
	fail_if(Tester::getRolledBack().size() != 0);
	fail_if(Tester::getCommitTs().size() != 0);
	//dodac w jednej, zatwierdzic, usunac w drugiej, zatwierdzic, sprawdzic czy trzecia wyczysci
	
	e = RootEntry(250, STORE_IXR_NULLVALUE, tid, 4);
	err = t->addEntry(c.get());
	test(err, "addEntry");
	
	IndexManager::getHandle()->begin(18);
	
	fail_if(Tester::getActive().size() != 2);
	
	
	IndexManager::getHandle()->commit(tid, 255);
	fail_if(Tester::getActive().size() != 1);
	fail_if(Tester::getCommitTs().size() != 1);
	tid = 18;
	
	e.modyfierTransactionId = tid;
	e.delTime = e.addTime + 1;
	e.addTime = STORE_IXR_NULLVALUE;
	err = t->addEntry(c.get());
	test(err, "addEntry");
	
	IndexManager::getHandle()->commit(tid, 256);
	
	fail_if(Tester::getActive().size() != 0);
	fail_if(Tester::getCommitTs().size() != 0);
	fail_if(Tester::getRolledBack().size() != 0);
	
	fail_if(node->size() == 0);
	
	tid = 19;
	IndexManager::getHandle()->begin(tid);
	
	transactionID.reset(new TransactionID(tid));
	
	sieve.reset(new EntrySieve(transactionID.get(), ih->getId()));
	err = constr->search(t.get(), sieve.get());
	test(err, "search");
	
	fail_if(Tester::getActive().size() != 1);
	fail_if(Tester::getCommitTs().size() != 0);
	fail_if(Tester::getRolledBack().size() != 0);
	
	fail_if(node->size() == 0);
	cleaner->clean(cnode, t->getNewComparator(), ih.get());
	fail_unless(node->size() == 0, "aktualny rozmiar wezla to: %d, a powinno byc 0", node->size());
	
	fail_if(Tester::getActive().size() != 1);
	fail_if(Tester::getCommitTs().size() != 0);
	fail_if(Tester::getRolledBack().size() != 0);
	
	t.release();
}

START_TEST (rollback) {
	Tester::testRollback();
}END_TEST

/*
START_TEST (complex_clean) {
	int err;
	auto_ptr<LogicalID> lid(new DBLogicalID(5));
	auto_ptr<IndexHandler> ih(new IndexHandler("d", "d", "d", lid.get()));
	auto_ptr<BTree> t(new BTree(new IntComparator(), ih.get()));
	
	err = t->createRoot();
	test(err, "createRoot");
	fail_if(Tester::getUsedCount() != 0, "tworzenie roota nie zwalnia wezlow");

	auto_ptr<Comparator> c (t->getNewComparator());
	err = c->setValue(2);
	auto_ptr<QueryNode> qnode(new IntNode(2));
	auto_ptr<Constraints> constr(new ExactConstraints(qnode.get()));
	
	tid_t tid = 
	auto_ptr<TransactionID> transactionID(new TransactionID(tid));
	
	auto_ptr<EntrySieve> sieve(new EntrySieve(transactionID.get()));
	CachedNode* cnode;
	Node* node;
	t->ih->getNode(cnode, Tester::getRoot(t.get()));
	cnode->getNode(node);
	
}END_TEST
*/

/** template

START_TEST (name) {
}END_TEST

*/

Suite * cleaning() {
	Suite *s = suite_create ("Cleaner");
	
	TCase *tc_core = tcase_create ("Cleaner");
	tcase_set_timeout(tc_core, 22); //timeout = 1s
	tcase_add_checked_fixture(tc_core, setup_listener, teardown_listener);
	tcase_add_test (tc_core, rollback);
   
    suite_add_tcase (s, tc_core);
	
	return s;
}
