#include <QueryExecutor/QueryResult.h>
#include <QueryExecutor/QueryExecutor.h>
#include <Indexes/IndexManager.h>
#include <Test/Indexes/Suits.h>

using namespace Errors;
using namespace Indexes;
using namespace QExecutor;

START_TEST (rootName) {
	QueryResult* res, *res2;
	auto_ptr<QueryResult> ares;
	int err;
	auto_ptr<IndexTesting::ConnectionThread> con(new IndexTesting::ConnectionThread());
	
	err = con->begin();
	test(err, "begin");
	
	err = con->process("create (20 as age) as emp", QueryResult::QBAG);
	test(err, "tworzenie obiektu");
	//ares.reset(res);
	
	err = con->process("emp.age", res);
	test(err, "tworzenie obiektu");
	ares.reset(res);
	
	fail_if(res->type() != QueryResult::QBAG, "zwrocono niewlasciwy typ");
	
	(dynamic_cast <QueryBagResult*> (res))->at(0, res2);
	fail_if(res2->type() != QueryResult::QREFERENCE, "zla zawartosc vectora: %d", res2->type());
	LogicalID* lid = (dynamic_cast <QueryReferenceResult*> (res2))->getValue();
	
	fail_if (lid->getParentRoot() != "emp", "zla nazwa roota: |%s|", lid->getParentRoot().c_str());
	
	err = con->end();
	test(err, "end");
	
}END_TEST

START_TEST (remove) {
	int err;
	auto_ptr<IndexTesting::ConnectionThread> con(new IndexTesting::ConnectionThread());
	
	err = con->begin();
	test(err, "begin");
	
	err = con->process("create (20 as age) as emp", QueryResult::QBAG);
	test(err, "tworzenie obiektu");
		
	err = con->process("create (20 as age) as not_emp", QueryResult::QBAG);
	test(err, "tworzenie obiektu");
	
	err = con->process("create int index emp_age on emp(age)", QueryResult::QNOTHING);
	test(err, "indeksowanie juz istniejacych obiektow");
	
	err = con->process("delete not_emp.age", QueryResult::QNOTHING);
	test(err, "usuwanie nieindeksowanego");
	
	err = con->process("delete emp.age", QueryResult::QNOTHING);
	fail_unless(err, "udalo sie usunac indeksowanego");
	
}END_TEST

/** template

START_TEST (name) {
}END_TEST

*/

Suite * parentName() {
	Suite *s = suite_create ("parentName");
	
	TCase *tc_core = tcase_create ("Btree operation");
	tcase_set_timeout(tc_core, 3); //timeout = 1s
	tcase_add_checked_fixture(tc_core, setup_listener, teardown_listener);
	tcase_add_test (tc_core, rootName);
	tcase_add_test (tc_core, remove);
	
   // tcase_add_test (tc_core, loop);
	//tcase_add_test_raise_signal(tc_core, segfault, SIGSEGV);
    suite_add_tcase (s, tc_core);
	
	return s;
}
