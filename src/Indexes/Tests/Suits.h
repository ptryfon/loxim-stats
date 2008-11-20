#ifndef __SUITS_H__
#define __SUITS_H__

#include <Indexes/const.h>
#include <Tester.h>
#include <Errors/Errors.h>
#include <ConnectionThread.h>
#include <check.h>
//#include <stdlib.h>
#include <signal.h>

//#include <Indexes/IndexHandler.h>


#define test(err,txt)  if (err) {fail_if(err, "%s, error code: %s", txt, Errors::SBQLstrerror(err)->c_str());} else {mark_point();}


#define ASSERT_SIG	SIGABRT

//extern string filename;

void setup_indexHandler();
void teardown_indexHandler();
void showmem(void* addr);
void setup(bool ifDelete, bool forceCrash = false);
void teardown(bool ifDelete);

const char* errmsg(int err);

//do testow integracyjnych
void teardown_listener();
void setup_listener();

//do testowania niejawnych optymalizacji 
string optResult(QueryParser* p, string query);

extern SBQLConfig *config;
extern ErrorConsole *con;
extern LogManager *lm;
extern DBStoreManager *sm;



Suite * node_buffer();
Suite * btree();
Suite * comparatorSuite();
Suite * traveler();
Suite * parentName();
Suite * syntax();
Suite * integration();
Suite * optimisation();
Suite * cleaning();
Suite * stateSaving();
Suite * dropping();
Suite * bugs();
#endif /*__SUITS_H__*/
