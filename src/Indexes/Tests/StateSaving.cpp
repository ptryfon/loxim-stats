#include "QueryExecutor/QueryResult.h"
#include "QueryExecutor/QueryExecutor.h"
#include "Indexes/IndexManager.h"
#include "Log/LogRecord.h"
//#include "Indexes/IndexManager.h"

#include "Suits.h"

using namespace Errors;
using namespace Indexes;
using namespace QExecutor;

void restart(bool forceCrash = false) {
	teardown(false);
	setup(false, forceCrash);
}

START_TEST (loxim_param_saving) {
	unsigned int t1, t2;
	tid_t tr1, tr2;
	t1 = LogRecord::getIdSeq();
	tr1 = Tester::getTransactionId();
	restart();
	t2 = LogRecord::getIdSeq();
	tr2 = Tester::getTransactionId();
	fail_if(t1 == t2);
	fail_unless(tr2 > tr1, "transactionId po starcie liczone sa od zera");
}END_TEST

void Tester::testIndexCount() {
	int err;
	QueryResult* res;
	auto_ptr<QueryResult> qr1, qr2;
	
	auto_ptr<IndexTesting::ConnectionThread> con(new IndexTesting::ConnectionThread());
	
	err = con->process("create int index a on b(c)", res);
	test(err, "create int index");
		
	fail_unless(res->type() == QueryResult::QNOTHING, "zwrocony bledny typ");
	
	err = con->begin();
	test(err, "begin");
	err = con->process("create (1 as c) as b");
	test(err, "create root");
	err= con->end();
	test(err, "end");
	
	err = con->process("create int index a2 on b(d)");
	fail_unless(err, "nie powinno byc mozliwosci zbudowania indeksu");
	
	err = con->begin();
	test(err, "begin");
	err = con->process("create (1 as c) as b");
	test(err, "create root");
	err= con->abort();
	test(err, "abort");
	
	con.reset(NULL);
	//mozna zrobic tu troche operacji zeby to nie byly domyslne wartosci. horyzont, first, last, dropped
	IndexManager* im = IndexManager::getHandle();
	fail_if(im->indexNames.size() != 1);
	IndexManager::string2index::iterator it;
	it = im->indexNames.begin();
	
	fail_if(it->first != "a");
	IndexHandler* ih = it->second;
	nodeAddress_t rootAddr = ih->tree->rootAddr;
	int rootLevel = ih->tree->rootLevel;
	fail_if(im->indexNames.size() != 1);
	int cmpType = ih->tree->defaultComparator->serialize();
	nodeAddress_t firstNode = ih->firstNode;
	nodeAddress_t lastNode = ih->lastNode;
	tid_t horizon = im->resolver->horizon;
	
	VisibilityResolver::rolledBack_t rolledB = im->resolver->rolledBack;
	fail_if(rolledB != im->resolver->rolledBack);
	fail_if (rolledB.size() != 1);
	fail_if (rolledB.begin()->second.begin()->second != 1);
	tid_t transaction = rolledB.begin()->second.begin()->first;
	fail_if (rolledB.begin()->second.size() != 1);
	fail_if(rolledB != im->resolver->rolledBack);
	nodeAddress_t nextFree = IndexHandler::nextFreeNodeAddr;
	nodeAddress_t dropped = IndexHandler::droppedNode;
	nodeAddress_t droppedLast = IndexHandler::droppedLastNode;
	fail_if(rolledB != im->resolver->rolledBack);
	
	restart();
	
	im = IndexManager::getHandle();
	fail_if(rolledB.size() != im->resolver->rolledBack.size(), "powinno byc %d a jest %d", rolledB.size(), im->resolver->rolledBack.size());
	fail_if(im->resolver->rolledBack.size() != 1);
	fail_if (im->resolver->rolledBack.begin()->second.size() != 1);
	
	/*
	VisibilityResolver::rolledBack_t::iterator rit = rolledB.begin();
	VisibilityResolver::rolledBackIndex_t::iterator iit;
	for (; rit != rolledB.end(); rit++) {
		cout << "\n-----------------" << endl << "index id: " << rit->first << endl;
		for (iit = rit->second.begin(); iit != rit->second.end(); iit++) {
			cout << iit->first << " -> " << iit->second << endl;
		}
	}
	
	cout << "resolver->rolledBack" << endl;
	
	rit = im->resolver->rolledBack.begin();
	for (; rit != im->resolver->rolledBack.end(); rit++) {
		cout << "\n-----------------" << endl << "index id: " << rit->first << endl;
		for (iit = rit->second.begin(); iit != rit->second.end(); iit++) {
			cout << iit->first << " -> " << iit->second << endl;
		}
	}
	*/
	
	fail_if(transaction != im->resolver->rolledBack.begin()->second.begin()->first);
	fail_if(rolledB != im->resolver->rolledBack);
	fail_if(im->indexNames.size() != 1, "ilosc indeksow: %d", im->indexNames.size());
	
	con.reset(new IndexTesting::ConnectionThread());
	
	err = con->process("create int index a on e(f)");
	fail_unless(err == EIndexExists | ErrIndexes, "powinien byc blad przy ponownym tworzeniu tego samego indeksu");
		
	err = con->process("create int index a2 on b(c)");
	fail_unless(err == (EFieldIndexed | ErrIndexes), "powinien byc blad przy ponownym tworzeniu indeksu na tym samym polu");
	
	im = IndexManager::getHandle();
	fail_if(im->indexNames.size() != 1);
	it = im->indexNames.begin();
	
	fail_if(it->first != "a");
	ih = it->second;
	fail_if(rootLevel != ih->tree->rootLevel, "rootLevel jest tracony. powinno byc %d a jest %d", rootLevel, ih->tree->rootLevel);
	fail_if(rootAddr != ih->tree->rootAddr, "adres roota jest tracony. powinno byc %d a jest %d", rootAddr, ih->tree->rootAddr);
	fail_if(cmpType != ih->tree->defaultComparator->serialize());
	fail_if(firstNode != ih->firstNode);
	fail_if(lastNode != ih->lastNode);
	fail_if(nextFree != IndexHandler::nextFreeNodeAddr);
	fail_if(dropped != IndexHandler::droppedNode);
	fail_if(droppedLast != IndexHandler::droppedLastNode);
	fail_unless(horizon <= im->resolver->horizon);
	
}

void Tester::testMultiRollback() {
	int err;
	QueryResult* res;
	auto_ptr<QueryResult> qr1, qr2;
	
	auto_ptr<IndexTesting::ConnectionThread> con(new IndexTesting::ConnectionThread());
	
	err = con->process("create int index a on b(c)", res);
	test(err, "create int index");
		
	fail_unless(res->type() == QueryResult::QNOTHING, "zwrocony bledny typ");
	delete res;
	
	err = con->process("create string index a2 on b(d)", res);
	test(err, "create int index");
	
	fail_unless(res->type() == QueryResult::QNOTHING, "zwrocony bledny typ");
	delete res;
	
	err = con->begin();
	test(err, "begin");
	err = con->process("create (1 as c, \"napis\" as d) as b", res);
	test(err, "create root");
	fail_unless(res->type() == QueryResult::QBAG, "zwrocony bledny typ");
	fail_unless(res->size() == 1);
	//cout << res->toString(0, true, "");
	delete res;
	
	err= con->abort();
	test(err, "rollback");
	
	IndexManager* im = IndexManager::getHandle();
	VisibilityResolver::rolledBack_t rolledB = im->resolver->rolledBack;
	
	fail_if(rolledB.size() != 2, "powinno byc 2 a jest %d", rolledB.size());
	fail_if(rolledB != im->resolver->rolledBack);
	
	restart();
	
	im = IndexManager::getHandle();
	/*
	VisibilityResolver::rolledBack_t::iterator rit = rolledB.begin();
		VisibilityResolver::rolledBackIndex_t::iterator iit;
		for (; rit != rolledB.end(); rit++) {
			cout << "\n-----------------" << endl << "index id: " << rit->first << endl;
			for (iit = rit->second.begin(); iit != rit->second.end(); iit++) {
				cout << iit->first << " -> " << iit->second << endl;
			}
		}
		
		cout << "resolver->rolledBack" << endl;
		
		rit = im->resolver->rolledBack.begin();
		for (; rit != im->resolver->rolledBack.end(); rit++) {
			cout << "\n-----------------" << endl << "index id: " << rit->first << endl;
			for (iit = rit->second.begin(); iit != rit->second.end(); iit++) {
				cout << iit->first << " -> " << iit->second << endl;
			}
		}
	*/
	fail_if(rolledB != im->resolver->rolledBack, "rolledB = %d, resolver = %d", rolledB.size(), im->resolver->rolledBack.size());
}

START_TEST (multi_rollback) {
	Tester::testMultiRollback();
}END_TEST

START_TEST (index_count) {
	Tester::testIndexCount();
}END_TEST

START_TEST (searching_reset) {
	int err;
	QueryResult* res;
	auto_ptr<QueryResult> qr1, qr2;
	
	auto_ptr<IndexTesting::ConnectionThread> con(new IndexTesting::ConnectionThread());
	auto_ptr<IndexTesting::ConnectionThread> con1(new IndexTesting::ConnectionThread());
	
	err = con->process("create int index a on b(c)", res);
	test(err, "create int index");
		
	fail_unless(res->type() == QueryResult::QNOTHING, "zwrocony bledny typ");
	
	err = con->begin();
	test(err, "begin");
	
	err = con1->begin();
	test(err, "begin");

	err = con1->process("create (1 as c, 2 as d) as b");
	test(err, "create root");
	
	err = con1->end();
	test(err, "end");
	//potem bedzie przesuniecie horyzontu 

	
	err = con1->begin();
	test(err, "begin");
		
	
	int count = 670;
	for (int i = 0; i < count; i++) {
		err = con->process("create (1 as c) as b");
		test(err, "create root");
		
		err = con1->process("create (1 as c, 2 as d) as b");
		test(err, "create root");
	}
	
	err= con->abort();
	test(err, "abort");
	//przesuniecie horyzontu
	
	err = con1->end();
	test(err, "end");

	err = con1->begin();
	test(err, "begin");

	err = con1->process("create (1 as c, 2 as d) as b");
	test(err, "create root");
	
	err = con1->end();
	test(err, "end");

	err = con1->begin();
	test(err, "begin");

	err = con1->process("index a = 1", res);
	test(err, "search");
	qr1.reset(res);
	fail_unless(qr1->type() == QueryResult::QBAG, "zwrocono inny obiekt niz oczekiwano");
	fail_if(qr1->size() != (unsigned int)count+2, "zwrocono inna liczba elementow niz oczekiwano");
	
	err = con1->end();
	test(err, "end");

	restart();
	
	con.reset(new IndexTesting::ConnectionThread());
	con1.reset(new IndexTesting::ConnectionThread());
	
	con1->begin();
	con->begin();
	
	int count2 = 30;
	for (int i = 0; i < count2; i++) {
		err = con->process("create (1 as c) as b");
		test(err, "create root");
		
		err = con1->process("create (1 as c, 2 as d) as b");
		test(err, "create root");
	}
	
	con->abort();
	con1->end();
	
	con->begin();
	
	err = con->process("index a = 1", res);
	test(err, "search");
	qr1.reset(res);
	fail_unless(qr1->type() == QueryResult::QBAG, "zwrocono inny obiekt niz oczekiwano");
	fail_if(qr1->size() != (unsigned int)count + 2 + count2, "zwrocono %d elementow, a oczekiwano %d", qr1->size(),(unsigned int)count + 2 + count2 );
	
	con->end();
	
	err = con->process("create int index a2 on b(d)", res);
	test(err, "create int index");
	
	restart(true); //awaria
	
	con.reset(new IndexTesting::ConnectionThread());
	con->begin();
		
	err = con->process("index a = 1", res);
	test(err, "search");
	qr1.reset(res);
	fail_unless(qr1->type() == QueryResult::QBAG, "zwrocono inny obiekt niz oczekiwano");
	fail_if(qr1->size() != (unsigned int)count + 2 + count2, "zwrocono inna liczba elementow niz oczekiwano");
	
	qr1.reset(NULL);
	err = con->process("index a2 = 2", res);
	test(err, "search");
	qr1.reset(res);
	fail_unless(qr1->type() == QueryResult::QBAG, "zwrocono inny obiekt niz oczekiwano");
	fail_if(qr1->size() != (unsigned int)count + 2 + count2, "zwrocono inna liczba elementow niz oczekiwano");
	
	con->end();
	
	
}END_TEST

/** template

START_TEST (name) {
}END_TEST

*/

Suite * stateSaving() {
	Suite *s = suite_create ("stateSaving");
	
	TCase *tc_core = tcase_create ("parameters_save");
	tcase_set_timeout(tc_core, 20); //timeout = 1s
	tcase_add_checked_fixture(tc_core, setup_listener, teardown_listener);
	tcase_add_test (tc_core, loxim_param_saving);
	tcase_add_test (tc_core, index_count);
	tcase_add_test (tc_core, searching_reset);
	tcase_add_test (tc_core, multi_rollback);
	
    suite_add_tcase (s, tc_core);
	
	return s;
}
