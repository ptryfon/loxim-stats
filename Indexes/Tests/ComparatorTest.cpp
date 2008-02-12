#include "Indexes/BTree.h"

#include "Suits.h"
#include <unistd.h>
#include <sstream>

using namespace Errors;
using namespace Indexes;


void showmem(void* addr) {
	int size = 40;
	size = min(size, NODE_SIZE);
	char* cadr = (char*)addr;
	cout << endl;
	for (int i = 0; i < size; i++) {
		cout << (unsigned int) cadr[i] << " ";
	}
	cout << endl;
}

START_TEST (setting_values) {

	int err;
	auto_ptr<Comparator> c(new IntComparator());
	
	RootEntry e(1,2,3,4);

	c->setEntry(&e);
	err = c->setValue(2.0);
	fail_unless(err, "Error missing");
	
	err = c->setValue("ds");
	fail_unless(err, "Error missing");
	
	err = c->setValue(2);
	test(err, "setValue(int)");


	c.reset(new StringComparator());
	string value = "wer";
	c->setValue(value);
	
	fail_if(value.size() != (uint) Tester::getStrSize(c.get()), "zle ustawiana jest dlugos tekstu");

}END_TEST


START_TEST (serializing) {
	
	char nodeMem[NODE_SIZE];
	
	Node* node = (Node*)&nodeMem;
	memset(&nodeMem, 0, NODE_SIZE);
	
	RootEntry* entry = (RootEntry*) node;
	
	auto_ptr<Comparator> c(new IntComparator());
	RootEntry e(1,2,3,4);
	
	c->setEntry(&e);
	int initValue = 8;
	c->setValue(initValue);
	
	//cout << c->toString();
	
	stringstream temp;
	temp << initValue;
	string initKey = temp.str();
	
	c->putValue(entry);
	
	string afterKey = c->keyToString(entry);
	
	fail_if(initKey != afterKey, "wyjeto inny klucz niz wlozono. wlozono |%s|, wyjeto |%s|", initKey.c_str(), afterKey.c_str());
	
	memset(&nodeMem, 0, NODE_SIZE);
	
//	showmem(node);
	
	c.reset(new StringComparator());
	string initv = "abaaaaa";
	c->setValue(initv);
	c->setEntry(&e);
	
	c->putValue(entry);

//	showmem(node);
	
	string result = c->keyToString(entry);
	
	fail_if(result != initv, "wyjeto inny klucz niz wlozono. wlozono |%s|, wyjeto |%s|", initv.c_str(), result.c_str());
	
}END_TEST

/** template

START_TEST (name) {
}END_TEST

*/


Suite * comparatorSuite() {
	Suite *s = suite_create ("ComparatorTest");
	
	TCase *tc_core = tcase_create ("values");
	tcase_set_timeout(tc_core, 1); //timeout = 1s
	tcase_add_test (tc_core, setting_values);
    tcase_add_test (tc_core, serializing);
    
   // tcase_add_test (tc_core, loop);
	//tcase_add_test_raise_signal(tc_core, segfault, SIGSEGV);
    suite_add_tcase (s, tc_core);
	
	return s;
}
