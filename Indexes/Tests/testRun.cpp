#include <time.h>
#include <stdlib.h>

#include "Suits.h"

#include <stdlib.h>

const char* errmsg(int err) {
	return Errors::SBQLstrerror(err)->c_str();
}

int main(int argc, const char* argv[]) {
	
	if (argc > 1) { //z parametrem bedzie no fork
		Tester::debugMode = true;
	}
	long seed = time(NULL); 
	srand(seed);
	
	//srand(0);
    Suite *s = suite_create (""); //dla klarownosci reszty kodu
    SRunner *sr = srunner_create (s);
    
    srunner_add_suite(sr, node_buffer());
    srunner_add_suite(sr, comparatorSuite());
    srunner_add_suite(sr, btree());
    srunner_add_suite(sr, traveler());
	srunner_add_suite(sr, parentName());
	srunner_add_suite(sr, syntax());
    srunner_add_suite(sr, integration());
    srunner_add_suite(sr, optimisation());
    srunner_add_suite(sr, cleaning());
    srunner_add_suite(sr, stateSaving());
    srunner_add_suite(sr, dropping());
    srunner_add_suite(sr, bugs());
    
    if (Tester::isDebugMode()) {
    	srunner_set_fork_status(sr, CK_NOFORK); //NOFORK - wspolna przestrzen adresowa - latwieszy debug, mniej stabilne
    }
    
    srunner_run_all (sr, CK_NORMAL);
    int number_failed;
    number_failed = srunner_ntests_failed (sr);
    srunner_free (sr);
    cout << "seed: " << seed << endl;
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
