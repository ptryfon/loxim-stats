#include "QueryExecutor/QueryResult.h"
#include "QueryExecutor/QueryExecutor.h"
#include "Indexes/IndexManager.h"
#include "Indexes/QueryOptimizer.h"
#include "Suits.h"

using namespace Errors;
using namespace Indexes;
using namespace QExecutor;

START_TEST (simple_syntax) {
	int err;
	TreeNode *tNode;
	auto_ptr<QueryParser> p(new QueryParser());
	auto_ptr<TreeNode> tree;
	QueryOptimizer::setImplicitIndexCall(false);
	
	
	err = p->parseIt("index emp_age <| 5", tNode);
	test(err, "blad parsowania");
	tree.reset(tNode);
	
	err = p->parseIt("index emp_age <| 5", tNode);
	test(err, "blad parsowania");
	tree.reset(tNode);
	
	err = p->parseIt("index emp_age <| \"ala\"", tNode);
	test(err, "blad parsowania");
	tree.reset(tNode);
	
	err = p->parseIt("index emp_age <| 2.5", tNode);
	test(err, "blad parsowania");
	tree.reset(tNode);
	mark_point();
	
	tree.reset(NULL);
	mark_point();
	err = p->parseIt("index emp_age <| 2,5", tNode);
	fail_unless(err, "powinien byc blad");
	//tree.reset(tNode);
	
	err = p->parseIt("index emp_age 2", tNode);
	fail_unless(err, "powinien byc blad");
		
	err = p->parseIt("index emp_age (| 5", tNode);
	test(err, "blad parsowania");
	tree.reset(tNode);

	err = p->parseIt("index emp_age = 5", tNode);
	test(err, "blad parsowania");
	tree.reset(tNode);
	
	err = p->parseIt("index emp_age (5) |>", tNode);
	test(err, "blad parsowania");
	tree.reset(tNode);
	
	err = p->parseIt("index emp_age 5 |)", tNode);
	test(err, "blad parsowania");
	tree.reset(tNode);
	
	err = p->parseIt("index emp_age 5|>", tNode);
	test(err, "blad parsowania");
	tree.reset(tNode);
	
	err = p->parseIt("index emp_age (|4 5 |>", tNode);
	test(err, "blad parsowania");
	tree.reset(tNode);
	
	err = p->parseIt("index emp_age <|4 5|)", tNode);
	test(err, "blad parsowania");
	tree.reset(tNode);
	
	err = p->parseIt("index emp_age <|4 5.6|)", tNode);
	test(err, "blad parsowania");
	tree.reset(tNode);

	err = p->parseIt("index emp_age <|4 \"ala\"|)", tNode);
	test(err, "blad parsowania");
	tree.reset(tNode);
	
}END_TEST

START_TEST (complex_query) {
	int err;
	TreeNode *tNode;
	auto_ptr<QueryParser> p(new QueryParser());
	auto_ptr<TreeNode> tree;
	QueryOptimizer::setImplicitIndexCall(false);
	
	err = p->parseIt("(index emp_age <| 5) where a = 3", tNode);
	test(err, "blad parsowania");
	tree.reset(tNode);
		
	err = p->parseIt("(index emp_age 5|)) where a = 3", tNode);
	test(err, "blad parsowania");
	tree.reset(tNode);
	
}END_TEST

START_TEST (subqueries) {
	int err;
	TreeNode *tNode;
	auto_ptr<QueryParser> p(new QueryParser());
	auto_ptr<TreeNode> tree;
	QueryOptimizer::setImplicitIndexCall(false);
	
	err = p->parseIt("index emp_age <| (5 + 5)", tNode);
	test(err, "blad parsowania");
	tree.reset(tNode);
	
	err = p->parseIt("index emp_age <| (\"ala\" + 4)", tNode);
	test(err, "blad parsowania");
	tree.reset(tNode);
	
	err = p->parseIt("index emp_age <| 2.5", tNode);
	test(err, "blad parsowania");
	tree.reset(tNode);
	mark_point();
	
	tree.reset(NULL);
	mark_point();
	err = p->parseIt("index emp_age (5 + 5)", tNode);
	fail_unless(err, "powinien byc blad");
	//tree.reset(tNode);
		
	err = p->parseIt("index emp_age (| (5)", tNode);
	test(err, "blad parsowania");
	tree.reset(tNode);

	err = p->parseIt("index emp_age = (5 + 7)", tNode);
	test(err, "blad parsowania");
	tree.reset(tNode);
	
	err = p->parseIt("index emp_age = 5 + 7", tNode);
	test(err, "blad parsowania");
	tree.reset(tNode);
	
	err = p->parseIt("(index emp_age = 5) + 7", tNode);
	test(err, "blad parsowania");
	tree.reset(tNode);
	
	err = p->parseIt("index emp_age (5) |>", tNode);
	test(err, "blad parsowania");
	tree.reset(tNode);
	
	err = p->parseIt("index emp_age (5+5) |)", tNode);
	test(err, "blad parsowania");
	tree.reset(tNode);
	
	err = p->parseIt("index emp_age 5 + 7 |>", tNode);
	test(err, "blad parsowania");
	tree.reset(tNode);
	
	err = p->parseIt("index emp_age (|4 + 5 5 + 7 |>", tNode);
	test(err, "blad parsowania");
	tree.reset(tNode);
	
	err = p->parseIt("index emp_age (|(4 + 5)(5 + 7)|>", tNode);
	test(err, "blad parsowania");
	tree.reset(tNode);
	
	err = p->parseIt("index emp_age <|4+6 8+5|)", tNode);
	test(err, "blad parsowania");
	tree.reset(tNode);
	
	err = p->parseIt("index emp_age <|7+4 5.6+\"ala\"|)", tNode);
	test(err, "blad parsowania");
	tree.reset(tNode);
	
}END_TEST

/** template

START_TEST (name) {
}END_TEST

*/

Suite * syntax() {
	Suite *s = suite_create ("Syntax");
	
	TCase *tc_core = tcase_create ("syntax");
	tcase_set_timeout(tc_core, 3); //timeout = 1s
	tcase_add_checked_fixture(tc_core, setup_listener, teardown_listener);
	tcase_add_test (tc_core, simple_syntax);
	tcase_add_test (tc_core, complex_query);
	tcase_add_test (tc_core, subqueries);
	
   // tcase_add_test (tc_core, loop);
	//tcase_add_test_raise_signal(tc_core, segfault, SIGSEGV);
    suite_add_tcase (s, tc_core);
	
	return s;
}
