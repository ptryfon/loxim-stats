#include <QueryExecutor/QueryResult.h>
#include <QueryExecutor/QueryExecutor.h>
#include <Indexes/IndexManager.h>
#include <Test/Indexes/Suits.h>

using namespace Errors;
using namespace Indexes;
using namespace QExecutor;

/* 
 * testowanie poprawek do znalezionych bledow
 * */

START_TEST (create_drop_lock) {
	int err;
	//QueryResult* res;
	auto_ptr<QueryResult> qr1, qr2;
	
	auto_ptr<IndexTesting::ConnectionThread> con(new IndexTesting::ConnectionThread());
	
	err = con->process("create int index a on b(c)");
	test(err, "tworzenie");
	
	err = con->process("delete index b");
	fail_if( err != (ENoIndex | ErrIndexes), "nie ma takiego indeksu, operaca nie mogla sie udac");
	
	err = con->process("delete index a");
	test(err, "usuwanie");
	
}END_TEST

START_TEST (query_tree_transform) {
	int err;
	string s1, s2;
	auto_ptr<QueryParser> p(new QueryParser());
	auto_ptr<IndexTesting::ConnectionThread> con(new IndexTesting::ConnectionThread());
	
	err = con->process("create int index emp_age on emp(age)", QueryResult::QNOTHING);
	test(err, "tworze indeks");
	
	s1 = optResult(p.get(), "emp where age <= 40 and 4 < age and ala = 5");
	s2 = optResult(p.get(), "(index emp_age (|4 to 40|>) where (ala = 5)");
	fail_if (s1 != s2, "rozne wyniki po sparsowaniu: s1=%s, s2=%s", s1.c_str(), s2.c_str());
}END_TEST

/** template

START_TEST (name) {
}END_TEST

*/

Suite * bugs() {
	Suite *s = suite_create ("Bugs");
	
	TCase *tc_core = tcase_create ("bugs");
	tcase_set_timeout(tc_core, 8); //timeout = 1s
	tcase_add_checked_fixture(tc_core, setup_listener, teardown_listener);
	tcase_add_test (tc_core, create_drop_lock);
	tcase_add_test (tc_core, query_tree_transform);
	
   // tcase_add_test (tc_core, loop);
	//tcase_add_test_raise_signal(tc_core, segfault, SIGSEGV);
    suite_add_tcase (s, tc_core);
	
	return s;
}
