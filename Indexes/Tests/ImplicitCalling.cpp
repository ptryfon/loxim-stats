#include "QueryExecutor/QueryResult.h"
#include "QueryExecutor/QueryExecutor.h"
#include "Indexes/IndexManager.h"
#include "Indexes/QueryOptimizer.h"
#include "Suits.h"

using namespace Errors;
using namespace Indexes;
using namespace QExecutor;

bool optPossible(QueryParser* p, bool possible, string query) {
	int err;
	TreeNode *tNode;
	QueryOptimizer::setImplicitIndexCall(false);
	err = p->parseIt(query, tNode);
	test(err, "blad parsowania");
	auto_ptr<TreeNode> tree(tNode);
	QueryOptimizer::setImplicitIndexCall(true);
	mark_point();
	auto_ptr<QueryOptimizer> opt(new QueryOptimizer());
	mark_point();
	Tester::whereReplace(opt.get(), tNode);
	mark_point();
	return Tester::optimizationPossible(opt.get()) == possible;
}

string optResult(QueryParser* p, string query) {
	int err;
	TreeNode *tNode;
	QueryOptimizer::setImplicitIndexCall(true);
	err = p->parseIt(query, tNode);
	test(err, ("blad parsowania: " + query).c_str());
	if (err) return "blad parsowania";
	return (dynamic_cast<QueryNode*>(tNode))->deParse();
}

START_TEST (possibility) {
	int err;
	bool ok;
	auto_ptr<QueryParser> p(new QueryParser());
	auto_ptr<IndexTesting::ConnectionThread> con(new IndexTesting::ConnectionThread());
	
	ok = optPossible(p.get(), false, "emp");
	fail_unless(ok, "bledne wyliczenie mozliwosci optymalizacji");
	
	ok = optPossible(p.get(), false, "emp where age = 7");
	fail_unless(ok, "bledne wyliczenie mozliwosci optymalizacji");
		
	err = con->process("create int index emp_age on emp(age)", QueryResult::QNOTHING);
	test(err, "tworze indeks");	
	
	err = con->process("create int index emp_name on emp(name)", QueryResult::QNOTHING);
	test(err, "tworze indeks");
	
	ok = optPossible(p.get(), false, "emp");
	fail_unless(ok, "bledne wyliczenie mozliwosci optymalizacji");
	
	ok = optPossible(p.get(), false, "emp where not_age = 7");
	fail_unless(ok, "bledne wyliczenie mozliwosci optymalizacji");
	
	ok = optPossible(p.get(), true, "emp where age = 7");
	fail_unless(ok, "bledne wyliczenie mozliwosci optymalizacji");
	
	ok = optPossible(p.get(), true, "emp where name = 7");
	fail_unless(ok, "bledne wyliczenie mozliwosci optymalizacji");
	
	ok = optPossible(p.get(), false, "emp where age2 = 7");
	fail_unless(ok, "bledne wyliczenie mozliwosci optymalizacji");
	
	ok = optPossible(p.get(), false, "emp where age := 7");
	fail_unless(ok, "bledne wyliczenie mozliwosci optymalizacji");
	
	ok = optPossible(p.get(), true, "emp where age2 > 3 and age = 7");
	fail_unless(ok, "bledne wyliczenie mozliwosci optymalizacji");
	
	ok = optPossible(p.get(), true, "emp where age2 > 3 and age = 7 and age3 = 3");
	fail_unless(ok, "bledne wyliczenie mozliwosci optymalizacji");
	
	ok = optPossible(p.get(), false, "emp where age2 > 3 and age = 7 or age3 = 3");
	fail_unless(ok, "bledne wyliczenie mozliwosci optymalizacji");
	
	ok = optPossible(p.get(), false, "emp where age2 > 3 and age = 7 and age4 < 6 or age3 = 3");
	fail_unless(ok, "bledne wyliczenie mozliwosci optymalizacji");

	ok = optPossible(p.get(), true, "emp where age2 > 3 and age = 7 and (age4 < 6 or age3 = 3)");
	fail_unless(ok, "bledne wyliczenie mozliwosci optymalizacji");
	
	ok = optPossible(p.get(), true, "emp where age2 > 3 and 7 = age and (age4 < 6 or age3 = 3)");
	fail_unless(ok, "bledne wyliczenie mozliwosci optymalizacji");
	
	ok = optPossible(p.get(), true, "emp where age2 > 3 and 7 < age and (age4 < 6 or age3 = 3) and 12 > age");
	fail_unless(ok, "bledne wyliczenie mozliwosci optymalizacji");
	
	ok = optPossible(p.get(), false, "emp where age = create 1 as emp");
	fail_unless(ok, "bledne wyliczenie mozliwosci optymalizacji");
	
	ok = optPossible(p.get(), true, "emp where age = (1)");
	fail_unless(ok, "bledne wyliczenie mozliwosci optymalizacji");
	
	//nie jest wykonywana optymalizacja jesli sa podzapytania, choc takie zapytanie mogloby byc zoptymalizowane
	ok = optPossible(p.get(), false, "emp where age = (1+2)");
	fail_unless(ok, "bledne wyliczenie mozliwosci optymalizacji");
	
	/*
	test(err, "begin");
	
	err = con->process("emp where age = 20", QueryResult::QBAG);
	test(err, "selekcja przed utworzeniem");
	
	err = con->process("create (20 as emp) as dept", QueryResult::QBAG);
	test(err, "tworzenie obiektu");
	
	err = con->process("emp where age = 20", QueryResult::QBAG);
	test(err, "selekcja po utworzeniu");
	
	err = con->end();
	test(err, "end");
	*/
}END_TEST

START_TEST (simple_optimisation) {
	int err;
	string s1, s2;
	auto_ptr<QueryParser> p(new QueryParser());
	auto_ptr<IndexTesting::ConnectionThread> con(new IndexTesting::ConnectionThread());
	
	err = con->process("create int index emp_age on emp(age)", QueryResult::QNOTHING);
	test(err, "tworze indeks");
	
	err = con->process("create int index emp_name on emp(name)", QueryResult::QNOTHING);
	test(err, "tworze indeks");
	
	s1 = optResult(p.get(), "emp");
	s2 = optResult(p.get(), "emp");
	fail_if (s1 != s2, "rozne wyniki po sparsowaniu: s1=%s, s2=%s", s1.c_str(), s2.c_str());
	
	s1 = optResult(p.get(), "emp where age = 3");
	s2 = optResult(p.get(), "index emp_age = 3");
	fail_if (s1 != s2, "rozne wyniki po sparsowaniu: s1=%s, s2=%s", s1.c_str(), s2.c_str());

	/*s1 = optResult(p.get(), "emp where age = 3");
	s2 = optResult(p.get(), "(index emp_age 3)");
	fail_if (s1 != s2, "rozne wyniki po sparsowaniu: s1=%s, s2=%s", s1.c_str(), s2.c_str());
	*/
	s1 = optResult(p.get(), "emp where age < 3");
	s2 = optResult(p.get(), "index emp_age 3 |)");
	fail_if (s1 != s2, "rozne wyniki po sparsowaniu: s1=%s, s2=%s", s1.c_str(), s2.c_str());
	
	s1 = optResult(p.get(), "emp where 3 > age");
	s2 = optResult(p.get(), "index emp_age 3 |)");
	fail_if (s1 != s2, "rozne wyniki po sparsowaniu: s1=%s, s2=%s", s1.c_str(), s2.c_str());
	
	s1 = optResult(p.get(), "emp where 3 > age and age > 1");
	s2 = optResult(p.get(), "index emp_age (|1 3 |)");
	fail_if (s1 != s2, "rozne wyniki po sparsowaniu: s1=%s, s2=%s", s1.c_str(), s2.c_str());
	
	s1 = optResult(p.get(), "emp where 3 >= age");
	s2 = optResult(p.get(), "index emp_age 3 |>");
	fail_if (s1 != s2, "rozne wyniki po sparsowaniu: s1=%s, s2=%s", s1.c_str(), s2.c_str());
	
	s1 = optResult(p.get(), "emp where 3 > age and age >= 1");
	s2 = optResult(p.get(), "index emp_age <|1 3 |)");
	fail_if (s1 != s2, "rozne wyniki po sparsowaniu: s1=%s, s2=%s", s1.c_str(), s2.c_str());
	
	s1 = optResult(p.get(), "emp where 3 < age");
	s2 = optResult(p.get(), "index emp_age (|3 ");
	fail_if (s1 != s2, "rozne wyniki po sparsowaniu: s1=%s, s2=%s", s1.c_str(), s2.c_str());
	
	s1 = optResult(p.get(), "emp where age > 3");
	s2 = optResult(p.get(), "index emp_age (|3 ");
	fail_if (s1 != s2, "rozne wyniki po sparsowaniu: s1=%s, s2=%s", s1.c_str(), s2.c_str());
	
	s1 = optResult(p.get(), "emp where 3 <= age");
	s2 = optResult(p.get(), "index emp_age <|3 ");
	fail_if (s1 != s2, "rozne wyniki po sparsowaniu: s1=%s, s2=%s", s1.c_str(), s2.c_str());
	
	s1 = optResult(p.get(), "emp where age >= 3");
	s2 = optResult(p.get(), "index emp_age <|3 ");
	fail_if (s1 != s2, "rozne wyniki po sparsowaniu: s1=%s, s2=%s", s1.c_str(), s2.c_str());

}END_TEST

START_TEST (complex_optimisation) {
	int err;
	string s1, s2;
	auto_ptr<QueryParser> p(new QueryParser());
	auto_ptr<IndexTesting::ConnectionThread> con(new IndexTesting::ConnectionThread());
	
	err = con->process("create int index emp_age on emp(age)", QueryResult::QNOTHING);
	test(err, "tworze indeks");
	
	err = con->process("create int index emp_name on emp(name)", QueryResult::QNOTHING);
	test(err, "tworze indeks");
	
	s1 = optResult(p.get(), "emp where age = 3 and age < 6");
	s2 = optResult(p.get(), "(index emp_age = 3) where age < 6");
	fail_if (s1 != s2, "rozne wyniki po sparsowaniu: s1=%s, s2=%s", s1.c_str(), s2.c_str());
	
	s1 = optResult(p.get(), "emp where age > 3 and age = 6");
	s2 = optResult(p.get(), "(index emp_age = 6) where age > 3");
	fail_if (s1 != s2, "rozne wyniki po sparsowaniu: s1=%s, s2=%s", s1.c_str(), s2.c_str());
	
	s1 = optResult(p.get(), "emp where age < 3 and not_age = 5 and age = 3 and not_age2 = 4 and age < 6");
	s2 = optResult(p.get(), "(index emp_age = 3) where age < 3 and not_age = 5 and not_age2 = 4 and age < 6");
	fail_if (s1 != s2, "rozne wyniki po sparsowaniu: s1=%s, s2=%s", s1.c_str(), s2.c_str());
	
	s1 = optResult(p.get(), "emp where age > 3 and not_age = 5 and age <= 6");
	s2 = optResult(p.get(), "(index emp_age (|3 6|>) where not_age = 5");
	fail_if (s1 != s2, "rozne wyniki po sparsowaniu: s1=%s, s2=%s", s1.c_str(), s2.c_str());
	
	//sprawdzenie czy toString dziala
	s1 = optResult(p.get(), "emp where age > 3 and not_age = 5 and age <= 6");
	s2 = optResult(p.get(), "(index emp_age (|3 6|>) where not_age2 = 5");
	fail_if (s1 == s2, "rozne zapytania daja ten sam toString: %s", s1.c_str());
	
}END_TEST

/** template

START_TEST (name) {
}END_TEST

*/

Suite * optimisation() {
	Suite *s = suite_create ("Optimisation");
	
	TCase *tc_core = tcase_create ("optimisation");
	tcase_set_timeout(tc_core, 3); //timeout = 1s
	tcase_add_checked_fixture(tc_core, setup_listener, teardown_listener);
	tcase_add_test (tc_core, possibility);
	tcase_add_test (tc_core, simple_optimisation);
	tcase_add_test (tc_core, complex_optimisation);
	
   // tcase_add_test (tc_core, loop);
	//tcase_add_test_raise_signal(tc_core, segfault, SIGSEGV);
    suite_add_tcase (s, tc_core);
	
	return s;
}
