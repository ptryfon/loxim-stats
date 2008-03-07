#include "QueryExecutor/QueryResult.h"
#include "QueryExecutor/QueryExecutor.h"
#include "Indexes/IndexManager.h"
//#include "Indexes/IndexManager.h"

#include "Suits.h"

using namespace Errors;
using namespace Indexes;
using namespace QExecutor;


SBQLConfig *config = NULL;
ErrorConsole *con;
LogManager *lm;
DBStoreManager *sm;

void deleteAll();
void unlock();

void configSection(const char* name) {
	bool first = true;
	if (config) {
		delete config;
		config = NULL;
		//SBQLConfig::startup();
		first = false;
	}
	config = new SBQLConfig(name);
	if (!first) {
		return;
	}
	int errorCode;
	errorCode = config->init("../../Server/Server.conf");
	test(errorCode, "Config init failed");
}

/* forceCrash domyslnie = false*/
void setup(bool ifDelete, bool forceCrash) {
	int errorCode;
	int err, value;
	SBQLConfig::startup();
	configSection("Errors");
	err = config->getInt("stderr", value);
	test(err, "config->getInt");
	con = new ErrorConsole("Server");
	errorCode=con->init(value);
	test(errorCode, "ErrorConsole init failed");
	
	if (ifDelete) {
		deleteAll();
	}
	
	lm = new LogManager();
	lm->init();
	sm = new DBStoreManager();
	sm->init(lm);
	LockManager::init();
	TransactionManager::init(sm, lm);
	sm->setTManager(TransactionManager::getHandle());
	sm->start();
	lm->start(sm);
	QueryBuilder::startup();
	mark_point();
	err = Indexes::IndexManager::init(forceCrash ? false : LogManager::isCleanClosed());
	test(err, "index init failed");
	ClassGraph::ClassGraph::init();

}

void setup_listener() {
	setup(true);
}

void teardown(bool ifDelete) {
	int err;
	err = Indexes::IndexManager::shutdown();
	test(err, "blad przy zamykaniu");
	QueryBuilder::shutdown();
	sm->stop();
    unsigned idx;
    lm->shutdown(idx);
    ClassGraph::ClassGraph::shutdown();
    delete TransactionManager::getHandle();
    config->free();
    delete config;
    config = NULL;
    SBQLConfig::startup();
    //Unlock();
    
    if (ifDelete) {
    	deleteAll();
    } else {
    	unlock();
    }
	
    con->free();
	config->free();
	
	delete sm;
	delete lm;
	delete con;
	delete config;
	config = NULL;
}

void teardown_listener() {
	teardown(true);
}

void deleteFile(const char* param_name) {
	int err, err2;
	string value;
	err = config->getString( param_name, value );
	stringstream buf, buf2;
	buf << "nie mozna pobrac parametru " << param_name;
	test(err, buf.str().c_str());
	err = unlink(value.c_str());
	err2 = errno;
	buf2 << "nie mozna usunac pliku " << value.c_str(); 
	test(err && (errno != ENOENT ), buf2.str().c_str());
}

void unlock() {
	configSection("Server");
	deleteFile("lockpath");
}

void deleteAll() {
	
	configSection("Store");
	deleteFile("store_file_default");
	deleteFile("store_file_map");
	deleteFile("store_file_roots");
	deleteFile("store_file_views");
	
	configSection("Backup");
	deleteFile("backup_file_default");
	deleteFile("backup_file_map");
	deleteFile("backup_file_roots");
	
	configSection("Server");
	deleteFile("lockpath");
	
	configSection("Log");
	deleteFile("logspath");
	
	configSection("Index");
	deleteFile("index_file");
	deleteFile("index_file_metadata");
}

QueryResult* bag(int value, string field, string root) {
	
	QueryResult *b, *i;
	i = new QueryIntResult(value);
	i = new QueryBinderResult(field, i);
	i = new QueryBinderResult(root, i);
	b = new QueryBagResult();
	b->addResult(i);
	return b;
}

START_TEST (create_and_index) {
	int err;
	QueryResult* res;
	auto_ptr<QueryResult> qr1, qr2;
	
	auto_ptr<IndexTesting::ConnectionThread> con(new IndexTesting::ConnectionThread());
	
	err = con->begin();
	test(err, "begin");
	
	err = con->process("create (20 as age) as emp", res);
	test(err, "dodanie obiektu");
	qr1.reset(res);
	fail_unless(qr1.get()->type() == QueryResult::QBAG, "zwrocono inny obiekt niz oczekiwano");
	
	err = con->end();
	test(err, "commit");
	
	//indeksowanie istniejacego
	err = con->begin();
	test(err, "begin");
	
	err = con->process("create int index emp_age on emp(age)", res);
	test(err, "indeksowanie juz istniejacych obiektow");
	qr1.reset(res);
	fail_unless(qr1.get()->type() == QueryResult::QNOTHING, "zwrocono inny obiekt niz oczekiwano");
	
	err = con->end();
	test(err, "commit");	
	
}END_TEST

START_TEST (index_and_create) {
	int err;
	QueryResult* res;
	auto_ptr<QueryResult> qr1, qr2;
	
	auto_ptr<IndexTesting::ConnectionThread> con(new IndexTesting::ConnectionThread());
	
	//dodanie do juz istniejacego indeksu
	err = con->begin();
	test(err, "begin");
		
	err = con->process("create int index emp_age on emp(age)", QueryResult::QNOTHING);
	test(err, "pusty indeks");
	
	err = con->end();
	test(err, "commit");

	err = con->begin();
	test(err, "begin");
	
	err = con->process("create (20 as age) as emp", res);
	test(err, "dodanie indeksowanego");
	qr1.reset(res);
	fail_unless(qr1->type() == QueryResult::QBAG, "zwrocono inny obiekt niz oczekiwano");
	
	err = con->process("index emp_age <| 5|", res);
	test(err, "wyszukanie po indeksie");
	qr1.reset(res);
	fail_unless(qr1->type() == QueryResult::QBAG, "zwrocono inny obiekt niz oczekiwano");
	fail_if(qr1->size() != 1, "zwrocono inna liczba elementow niz oczekiwano");
	
	err = con->process("index emp_age <| (4+1)|", res);
	test(err, "wyszukanie po indeksie");
	qr1.reset(res);
	fail_unless(qr1->type() == QueryResult::QBAG, "zwrocono inny obiekt niz oczekiwano");
	fail_if(qr1->size() != 1, "zwrocono inna liczba elementow niz oczekiwano");
	
	err = con->process("index emp_age = 5", res);
	test(err, "wyszukanie po indeksie");
	qr1.reset(res);
	fail_unless(qr1->type() == QueryResult::QBAG, "zwrocono inny obiekt niz oczekiwano");
	fail_if(qr1->size() != 0, "zwrocono inna liczba elementow niz oczekiwano");
	
	err = con->process("index emp_age |5 |>", res);
	test(err, "wyszukanie po indeksie");
	qr1.reset(res);
	fail_unless(qr1->type() == QueryResult::QBAG, "zwrocono inny obiekt niz oczekiwano");
	fail_if(qr1->size() != 0, "zwrocono inna liczba elementow niz oczekiwano");
		
	err = con->process("index emp_age <| 4 to 5 |>", res);
	test(err, "wyszukanie po indeksie");
	qr1.reset(res);
	fail_unless(qr1->type() == QueryResult::QBAG, "zwrocono inny obiekt niz oczekiwano");
	fail_if(qr1->size() != 0, "zwrocono inna liczba elementow niz oczekiwano");
		
	err = con->end();
	test(err, "commit");
	
}END_TEST

START_TEST (listing_index) {
	int err;
	auto_ptr<IndexTesting::ConnectionThread> con(new IndexTesting::ConnectionThread());
	
	QueryResult *qres;
	
	err = con->process("index", qres);
	test(err, "pusty indeks");
	
	string empty = qres->toString(0, true, ""); 
	//cout << empty;
	
	delete qres;
	
	err = con->process("create int index emp_age on emp(age)", QueryResult::QNOTHING);
	test(err, "pusty indeks");
	
	err = con->process("index", qres);
	test(err, "pusty indeks");
	
	string emp_age = qres->toString(0, true, "");
	//cout << endl << one << endl;
	
	delete qres;
	
	fail_if(emp_age == empty);
	
	err = con->process("create string index city_name on city(name)", QueryResult::QNOTHING);
	test(err, "pusty indeks");
	
	err = con->process("index", qres);
	test(err, "pusty indeks");
	
	string city_name = qres->toString(0, true, "");
	//cout << endl << one << endl;
	
	delete qres;
	
	fail_if(city_name == emp_age);
	fail_if(empty == city_name);
	
	err = con->process("delete index city_name", QueryResult::QNOTHING);
	test(err, "pusty indeks");
	
	err = con->process("index", qres);
	test(err, "pusty indeks");
	
	string removed2 = qres->toString(0, true, "");
	//cout << endl << removed << endl;
	
	delete qres;
	
	fail_if(emp_age != removed2);
	
	err = con->process("delete index emp_age", QueryResult::QNOTHING);
	test(err, "pusty indeks");
	
	err = con->process("index", qres);
	test(err, "pusty indeks");
	
	string removed1 = qres->toString(0, true, "");
	//cout << endl << removed << endl;
	
	delete qres;
	
	fail_if(empty != removed1);
	
}END_TEST
	
START_TEST (content_possibilities) {
	int err;
	auto_ptr<IndexTesting::ConnectionThread> con(new IndexTesting::ConnectionThread());
	
	err = con->begin();
	test(err, "begin");
		
	err = con->process("create int index emp_age on emp(age)", QueryResult::QNOTHING);
	test(err, "pusty indeks");
	
	err = con->end();
	test(err, "commit");
	
	err = con->begin();
	test(err, "begin");
	
	err = con->process("create (20 as age, 3 as age) as emp", QueryResult::QNOTHING);
	fail_unless(err, "powinien byc blad. obiekt ma dwie indeksowane wartosci");
	//tutaj nastapi rollback
	
	err = con->begin();
	test(err, "begin");
	
	err = con->process("create (20 as not_age) as emp", QueryResult::QNOTHING);
	fail_unless(err, "powinien byc blad. obiekt powinien miec indeksowana wartosc");
	
	err = con->end();
	test(err, "commit");
	
}END_TEST

START_TEST (unique) {
	QueryResult* res;
	int err;
	auto_ptr<IndexTesting::ConnectionThread> con(new IndexTesting::ConnectionThread());
	
	err = con->process("ala", res);
	test(err, "ala");
	
	fail_unless(res->type() == QueryResult::QNOTHING, "zwrocony bledny typ");
	
	err = con->process("create int index a on b(c)", res);
	test(err, "create int index");
	
	fail_unless(res->type() == QueryResult::QNOTHING, "zwrocony bledny typ");
	
	err = con->process("create int index a on e(f)");
	fail_unless(err == EIndexExists | ErrIndexes, "powinien byc blad przy ponownym tworzeniu tego samego indeksu");
	
	err = con->process("create int index a2 on b(c)");
	fail_unless(err == EFieldIndexed | ErrIndexes, "powinien byc blad przy ponownym tworzeniu indeksu na tym samym polu");
	
}END_TEST

START_TEST (multi) {
	int err;
	auto_ptr<IndexTesting::ConnectionThread> con(new IndexTesting::ConnectionThread());
	
	err = con->begin();
	test(err, "begin");
	
	err = con->process("create (20 as heigh, 3 as age) as emp");
	test(err, "tworzenie");
		
	err = con->end();
	test(err, "commit");
	
	err = con->process("create int index a on emp(age)");
	test(err, "create int index");
	
	err = con->process("create int index a2 on emp(heigh)");
	test(err, "create int index");
	
}END_TEST

void Tester::visibilityMap() {
	int err;
	IndexManager* im = IndexManager::getHandle();
	fail_if(im->resolver->rolledBack.size() != 0);
	
	auto_ptr<IndexTesting::ConnectionThread> con(new IndexTesting::ConnectionThread());
	
	fail_if(im->resolver->rolledBack.size() != 0);
	
	err = con->process("create int index a on emp(age)");
	test(err, "create int index");
	
	fail_if(im->resolver->rolledBack.size() != 0);
	
	err = con->process("create int index a2 on emp(not_age)");
	test(err, "create int index");
	
	fail_if(im->resolver->rolledBack.size() != 0);
	
	err = con->process("create int index a3 on not_emp(age)");
	test(err, "create int index");
	
	fail_if(im->resolver->rolledBack.size() != 0);
	
	err = con->process("delete index a3");
	test(err, "delete index");
	
	fail_if(im->resolver->rolledBack.size() != 0);
	
	err = con->begin();
	test(err, "begin");
	
	fail_if(im->resolver->rolledBack.size() != 0);
	
	err = con->process("create (20 as age, 3 as not_age) as emp");
	test(err, "tworzenie");
	
	fail_if(im->resolver->rolledBack.size() != 0);
	
	err = con->abort();
	test(err, "rollback");
	
	fail_if(im->resolver->rolledBack.size() != 2);
	VisibilityResolver::rolledBack_t::const_iterator it = im->resolver->rolledBack.begin();
	for (; it != im->resolver->rolledBack.end(); it++) {
		fail_if(it->second.size() != 1);
	}
	
	err = con->process("delete index a2");
	test(err, "delete index");
	
	fail_if(im->resolver->rolledBack.size() != 1);
	fail_if(im->resolver->rolledBack.begin()->second.size() != 1);
	
}

START_TEST (visibility_map) {
	Tester::visibilityMap();
}END_TEST

void Tester::testHorizonMove() {
	auto_ptr<VisibilityResolver> v(new VisibilityResolver());
	tid_t h;
	
	fail_if(v->horizon >= 10, "horyzont: %d", v->horizon);
	h = v->horizon;
	
	v->begin(10);
	v->entryAdded(10, 1); //udawajmy ze transakcja nr 10 zmienila indeks 1
	
	fail_if(v->horizon != h, "horyzont: %d", v->horizon);
	
	v->begin(20);
	v->entryAdded(20, 1); //udawajmy ze transakcja nr 20 zmienila indeks 1
	
	fail_if(v->horizon != h, "horyzont: %d", v->horizon);
	
	v->begin(30);
	v->entryAdded(30, 1);
	
	fail_if(v->horizon != h, "horyzont: %d", v->horizon);
	
	v->commit(10, 40);
	
	fail_if(v->horizon != h, "horyzont: %d", v->horizon);
	
	v->begin(50);
	v->entryAdded(50, 1);
	
	fail_if(v->horizon != h, "horyzont: %d", v->horizon);
	
	v->begin(60);
	v->entryAdded(60, 1);
	
	fail_if(v->horizon != h, "horyzont: %d", v->horizon);
	
	v->commit(60, 70);
	
	fail_if(v->horizon != h, "horyzont: %d", v->horizon);
	
	v->commit(20, 80);
	
	fail_if(v->horizon != h, "horyzont: %d", v->horizon);
	
	v->begin(90);
	v->entryAdded(90, 1);
	
	fail_if(v->horizon != h, "horyzont: %d", v->horizon);
	
	v->commit(50, 100);
	
	fail_if(v->horizon != h, "horyzont: %d", v->horizon);
	
	v->commit(30, 110);
	
	fail_if(v->horizon != 60, "horyzont: %d", v->horizon);
	
}

START_TEST (horizonMove) {
	Tester::testHorizonMove();
}END_TEST

START_TEST (visibility) {
	int len;
	bool visible;
	auto_ptr<VisibilityResolver> v(new VisibilityResolver());
	
	v->begin(10);
	v->begin(20);
	v->begin(30);
	
	v->entryAdded(10, 1); //udawajmy ze transakcja nr 10 zmienila indeks 1
	v->commit(10, 40);
	
	v->begin(50);
	
	visible = v->isVisible(10, 20, 1);
	fail_if(visible, "zle obliczenie widocznosci");
	
	visible = v->isVisible(10, 30, 1);
	fail_if(visible, "zle obliczenie widocznosci");
	
	visible = v->isVisible(10, 41, 1);
	fail_if(!visible, "zle obliczenie widocznosci");
	
	len = Tester::commitMapSize(v.get());
	fail_if(len != 1, "zle zwalnianie mapy %d", len);
	
	v->entryAdded(20, 1); //udawajmy ze transakcja nr 20 zmienila indeks
	v->commit(20, 60);
	
	len = Tester::commitMapSize(v.get());
	fail_if(len != 2, "zle zwalnianie mapy %d", len);
	
	v->entryAdded(30, 1); //udawajmy ze transakcja nr 30 zmienila indeks
	v->commit(30, 70);
	
	len = Tester::commitMapSize(v.get());
	fail_if(len != 2, "zle zwalnianie mapy");
	
	visible = v->isVisible(10, 50, 1);
	fail_if(!visible, "zle obliczenie widocznosci");
	
	v.reset(new VisibilityResolver());
	
	v->begin(10);
	v->begin(20);
	v->begin(30);

	v->entryAdded(20, 1); //udawajmy ze transakcja nr 20 zmienila indeks
	v->commit(20, 40);
	v->entryAdded(30, 1); //udawajmy ze transakcja nr 30 zmienila indeks
	v->commit(30, 40);
	
	v->begin(50);
	
	len = Tester::commitMapSize(v.get());
	fail_if(len != 2, "zle zwalnianie mapy");
	
	visible = v->isVisible(20, 10, 1);
	fail_if(visible, "zle obliczenie widocznosci");
	
	visible = v->isVisible(10, 50, 1);
	fail_if(visible, "zle obliczenie widocznosci");
	
	v->entryAdded(10, 1); //udawajmy ze transakcja nr 10 zmienila indeks
	v->commit(10, 60);
	
	len = Tester::commitMapSize(v.get());
	fail_if(len != 1, "zle zwalnianie mapy %d", len);
	
	v->commit(50, 70);
	
	len = Tester::commitMapSize(v.get());
	fail_if(len != 0, "zle zwalnianie mapy %d", len);
} END_TEST

void Tester::addRollback() {
	int err = 0;
	auto_ptr<LogicalID> dblid(new DBLogicalID(5));
	auto_ptr<IndexHandler> ih(new IndexHandler("d", "d", "d", dblid.get()));
	BTree *t = new BTree(new StringComparator(), ih.get());
	ih->tree = t;
	auto_ptr<Comparator> c(t->getNewComparator());
	
	err = t->createRoot();
	test(err, "createRoot");
	
	VisibilityResolver* v = IndexManager::getHandle()->resolver.get();
		
	lid_t lid = 3;
	ts_t tstamp = 11;
	v->begin(10);
	RootEntry e(tstamp++, STORE_IXR_NULLVALUE, 10, lid++);
	c->setEntry(&e);
	
	int rolledBack = 860, inside = 790;
	
	
	for (int i = 0; i < rolledBack; i++) {
		e.logicalID++;
		err = c->setValue(6);
		test(err, "ustawienie klucza");
		
		err = t->addEntry(c.get());
		test(err, "addEntry");
	}
	
	v->abort(10);
	
	e.logicalID = 3;
	e.modyfierTransactionId = 20;
	v->begin(20);
	
	for (int i = 0; i < inside; i++) {
		e.logicalID++;
		err = c->setValue(6);
		test(err, "ustawienie klucza");
		
		err = t->addEntry(c.get());
		test(err, "addEntry");
	}
	
	auto_ptr<QueryNode> inode(new IntNode(6));
	auto_ptr<Constraints> constr(new ExactConstraints(inode.get()));
	
	auto_ptr<TransactionID> tid(new TransactionID(20));
	EntrySieve sieve(tid.get(), ih->getId());
	
	mark_point();
	err = constr->search(t, &sieve);
	test(err, "find");
	
	err = sieve.getResult()->size();
	fail_if(err != inside, "zla ilosc przesianych elementow");
	
	v->abort(20);
}

START_TEST (add_rollback) {
	Tester::addRollback();
} END_TEST

START_TEST (isolation) {
	QueryResult* res;
	auto_ptr<QueryResult> qr1;
	int err;
	auto_ptr<IndexTesting::ConnectionThread> con1(new IndexTesting::ConnectionThread());
	auto_ptr<IndexTesting::ConnectionThread> con2(new IndexTesting::ConnectionThread());
	
	err = con1->begin();
	test(err, "begin");
		
	err = con1->process("create int index emp_age on emp(age)", QueryResult::QNOTHING);
	test(err, "pusty indeks");
	
	err = con1->end();
	test(err, "commit");
	
	//w jednej transakcji dodajemy, w drugiej odczytujemy
	err = con1->begin();
	test(err, "begin");
		
	err = con1->process("create (20 as age) as emp", QueryResult::QBAG);
	test(err, "pusty indeks");
	
	err = con2->begin();
	test(err, "begin");
	
	err = con2->process("index emp_age = 20", res);
	test(err, "zliczanie indeksow");
	
	qr1.reset(res);
	fail_unless(qr1->type() == QueryResult::QBAG, "zwrocono inny obiekt niz oczekiwano");
	fail_if(qr1->size() != 0, "zwrocono inna liczba elementow niz oczekiwano");
		
	err = con1->end();
	test(err, "commit");
	
	//transakcja dodajaca zacommitowana
	
	err = con2->process("index emp_age = 20", res);
	test(err, "zliczanie indeksow");
	
	qr1.reset(res);
	fail_unless(qr1->type() == QueryResult::QBAG, "zwrocono inny obiekt niz oczekiwano. %d", qr1->type());
	fail_if(qr1->size() != 0, "zwrocono inna liczba elementow niz oczekiwano");
	
	err = con2->end();
	test(err, "commit");
	
	//w transakcji rozpoczetej teraz powinny juz byc widoczne dodane rooty
	
	err = con2->begin();
	test(err, "begin");
		
	err = con2->process("index emp_age = 20", res);
	test(err, "zliczanie indeksow");
		
	qr1.reset(res);
	fail_unless(qr1->type() == QueryResult::QBAG, "zwrocono inny obiekt niz oczekiwano");
	fail_if(qr1->size() != 1, "zwrocono inna liczba elementow niz oczekiwano. %d", qr1->size());
	err = con2->end();
	test(err, "commit");
	
}END_TEST

//bez watkow nie przetestuje: tworzenie indeksu gdy mamy rozne obiekty ktore beda indeksowane i sa one aktualnie w roznych fazach zacommitowania (w tym te ktore rzuca blad)

START_TEST (root_remove) {
	QueryResult* res;
	auto_ptr<QueryResult> qr1;
	int err;
	auto_ptr<IndexTesting::ConnectionThread> con1(new IndexTesting::ConnectionThread());
	auto_ptr<IndexTesting::ConnectionThread> con2(new IndexTesting::ConnectionThread());

	err = con1->process("create int index emp_age on emp(age)", QueryResult::QNOTHING);
	test(err, "pusty indeks");
	
	err = con1->begin();
	test(err, "begin");
	
	err = con1->process("create (20 as age) as emp", QueryResult::QBAG);
	test(err, "dodanie obiektu");
	
	err = con1->end();
	test(err, "commit");
	
	err = con1->begin();
	test(err, "begin");
	
	err = con2->begin();
	test(err, "begin");
	
	err = con1->process("index emp_age = 20", res);
	test(err, "wyszukanie po indeksie");
	qr1.reset(res);
	fail_unless(qr1->type() == QueryResult::QBAG, "zwrocono inny obiekt niz oczekiwano");
	fail_if(qr1->size() != 1, "zwrocono inna liczba elementow niz oczekiwano");
	
	err = con2->process("index emp_age = 20", res);
	test(err, "wyszukanie po indeksie");
	qr1.reset(res);
	fail_unless(qr1->type() == QueryResult::QBAG, "zwrocono inny obiekt niz oczekiwano");
	fail_if(qr1->size() != 1, "zwrocono inna liczba elementow niz oczekiwano");
	
	err = con1->process("(index emp_age = 20).age", res);
	test(err, "wyszukanie po indeksie");
	qr1.reset(res);
	fail_unless(res->size() == 1);
	
	err = con1->process("delete emp", res);
	test(err, "suwanie roota");
	qr1.reset(res);
	
	err = con1->process("index emp_age = 20", res);
	test(err, "wyszukanie po indeksie");
	qr1.reset(res);
	fail_unless(qr1->type() == QueryResult::QBAG, "zwrocono inny obiekt niz oczekiwano");
	fail_if(qr1->size() != 0, "zwrocono inna liczba elementow niz oczekiwano");
	
	err = con1->process("(index emp_age = 20).age", res);
	test(err, "wyszukanie po indeksie");
	qr1.reset(res);
	fail_unless(res->size() == 0);
	//cout << res->toString(0, true, "") << endl;
	
	err = con2->process("index emp_age = 20", res);
	test(err, "wyszukanie po indeksie");
	qr1.reset(res);
	fail_unless(qr1->type() == QueryResult::QBAG, "zwrocono inny obiekt niz oczekiwano");
	fail_if(qr1->size() != 1, "zwrocono inna liczba elementow niz oczekiwano");
	
	err = con2->process("(index emp_age = 20).age", res);
	fail_if(err != (ENoObject | ErrTManager)); //store usuwa rooty nawet pomiedzy transakcjami
	qr1.reset(res);
	
	//cout << res->toString(0, true, "");
	
}END_TEST

START_TEST (modify) {
	
	QueryResult* res;
	auto_ptr<QueryResult> qr1;
	int err;
	auto_ptr<IndexTesting::ConnectionThread> con1(new IndexTesting::ConnectionThread());
	auto_ptr<IndexTesting::ConnectionThread> con2(new IndexTesting::ConnectionThread());
	
	err = con1->begin();
	test(err, "begin");
	
	err = con1->process("create (20 as age) as emp", QueryResult::QBAG);
	test(err, "dodanie obiektu");
	
	err = con1->end();
	test(err, "commit");
	
	err = con1->process("create int index emp_age on emp(age)", QueryResult::QNOTHING);
	test(err, "tworzenie indeksu");
	
	err = con1->begin();
	test(err, "begin");
	
	err = con1->process("emp := (30 as not_age)");
	fail_unless(err, "nieprawidlowe modify object");
	//zerwanie transakcji
	err = con1->begin();
	test(err, "begin");
	err = con1->process("emp := (30 as age)", res);
	
	qr1.reset(res);
	test(err, "modyfikacja");
	fail_unless(qr1->type() == QueryResult::QNOTHING, "zwrocono inny obiekt niz oczekiwano");
	//fail_if(qr1->size() != 1, "zwrocono inna liczba elementow niz oczekiwano. %d", qr1->size());

	err = con1->process("index emp_age <|10 to 40|>", res);
	test(err, "zliczanie indeksow");
		
	qr1.reset(res);
	fail_unless(qr1->type() == QueryResult::QBAG, "zwrocono inny obiekt niz oczekiwano");
	fail_if(qr1->size() != 1, "zwrocono inna liczba elementow niz oczekiwano. %d", qr1->size());
	
	err = con1->process("index emp_age = 30", res);
	test(err, "zliczanie indeksow");
		
	qr1.reset(res);
	fail_unless(qr1->type() == QueryResult::QBAG, "zwrocono inny obiekt niz oczekiwano");
	fail_if(qr1->size() != 1, "zwrocono inna liczba elementow niz oczekiwano. %d", qr1->size());
	
	err = con1->process("delete emp", res);
	test(err, "suwanie roota");
	qr1.reset(res);
	
	err = con1->process("index emp_age = 30", res);
	test(err, "zliczanie indeksow");
	qr1.reset(res);
	
	fail_if(qr1->size() != 0, "zle zlicza obiekty po usunieciu");
	
	err = con1->process("create (20 as age) as emp", res);
	test(err, "ponowne utworzenie roota");
	qr1.reset(res);
	
	err = con1->process("emp.age", res);
	test(err, "pytanie o zagniezdzone");
	qr1.reset(res);
	
	fail_if(qr1->size() != 1);
	
	err = con1->process("delete emp.age", res);
	fail_unless(err, "powinien byc blad");
	qr1.reset(res);
	
	//byl blad wiec rollback
	
	err = con1->begin();
	test(err, "begin");
	
	//fail_if(qr1->size() != 1);
	
	err = con1->process("create (20 as age) as not_emp", res);
	test(err, "ponowne utworzenie roota");
	qr1.reset(res);
	
	err = con1->process("create (30 as age) as not_emp", res);
	test(err, "ponowne utworzenie roota");
	qr1.reset(res);
	
	err = con1->process("not_emp.age", res);
	test(err, "pytanie o zagniezdzone");
	qr1.reset(res);
	 
	fail_if(qr1->size() != 2, "oczekiwano 2 obiektow, otrzymano: %d", qr1->size());
	
	err = con1->process("delete (not_emp where age = 30).age", res);
	test(err, "usuwanie");
	qr1.reset(res);
	
	err = con1->process("not_emp.age", res);
	test(err, "pytanie o zagniezdzone");
	qr1.reset(res);
	 
	fail_if(qr1->size() != 1, "oczekiwano 1 obiektu, otrzymano: %d", qr1->size());
	
	err = con1->process("not_emp", res);
	test(err, "pytanie o rooty");
	qr1.reset(res);
	 
	fail_if(qr1->size() != 2);
	
	err = con1->process("delete not_emp where age != 20", res);
	test(err, "pytanie o rooty");
	qr1.reset(res);
	 
	err = con1->process("not_emp", res);
	test(err, "pytanie o rooty");
	qr1.reset(res);
	
	fail_if(qr1->size() != 1);
	
	err = con1->process("delete not_emp", res);
	test(err, "pytanie o rooty");
	qr1.reset(res);
	
	err = con1->process("not_emp", res);
	test(err, "pytanie o rooty");
	qr1.reset(res);
	
	fail_if(qr1->size() != 0);
	
/*	
	err = con1->process("emp := (30 as not_age)", res);
	fail_unless(err, "powinien zabronic zmiany struktury");
*/
	
	err = con1->end();
	test(err, "commit");
	
}END_TEST

START_TEST (setup_teardown) {
}END_TEST

START_TEST (subqueries_calculation) {
	QueryResult* res;
	auto_ptr<QueryResult> qr1;
	int err;
	auto_ptr<IndexTesting::ConnectionThread> con1(new IndexTesting::ConnectionThread());
	auto_ptr<IndexTesting::ConnectionThread> con2(new IndexTesting::ConnectionThread());
	
	err = con1->process("create int index emp_age on emp(age)", QueryResult::QNOTHING);
	test(err, "tworzenie indeksu");
	
	err = con1->begin();
	test(err, "begin");
	
	err = con1->process("create (20 as age) as emp", QueryResult::QBAG);
	test(err, "dodanie obiektu");
	
	err = con1->process("create (20 as age) as not_emp", QueryResult::QBAG);
	test(err, "dodanie obiektu");
	
	err = con1->process("emp where age = (not_emp.age)", res);
	test(err, "zliczanie indeksow");
	qr1.reset(res);
	
	fail_if(qr1->size() != 1, "zle zlicza obiekty: %d obiektow", qr1->size());
		
	err = con1->process("index emp_age = (not_emp.age)", res);
	test(err, "zliczanie indeksow");
	qr1.reset(res);
	
	fail_if(qr1->size() != 1, "zle zlicza obiekty");
	
	err = con1->process("create (20 as age) as not_emp", QueryResult::QBAG);
	test(err, "dodanie obiektu");
	
	err = con1->process("emp where age = (not_emp.age)", res);
	test(err, "zliczanie indeksow");
	qr1.reset(res);
	
	fail_if(qr1->size() != 0, "zle zlicza obiekty: %d obiektow", qr1->size());
	
	err = con1->process("index emp_age = (not_emp.age)", res);
	fail_if(err != (EOperNotDefined | ErrIndexes), "taki obiekt powinien zwrocic blad");
	
	err = con1->process("emp.age = 20", res);
	test(err, "test czy nadal wewnatrz transakcji");
	qr1.reset(res);
	fail_if(res->type() != QueryResult::QBOOL, "poza transakcja");
	
	//bedny typ podzapytania
	err = con1->process("index emp_age = \"ala\"", QueryResult::QBAG);
	fail_if(err != (EBadValue | ErrIndexes), "powinien byc blad - bledny typ podzapytania");
	
	err = con1->process("emp.age = 20", res);
	test(err, "test czy nadal wewnatrz transakcji");
	qr1.reset(res);
	fail_if(res->type() != QueryResult::QBOOL, "poza transakcja");
	
	err = con1->end();
	test(err, "commit");
		
}END_TEST

/** template

START_TEST (name) {
}END_TEST

*/

Suite * integration() {
	Suite *s = suite_create ("integration");
	
	TCase *tc_no_startup = tcase_create("visibility");
	//tcase_add_checked_fixture(tc_no_startup, setup_listener, teardown_listener);//tego nie bylo
	
	tcase_add_test (tc_no_startup, visibility);
	
	TCase *tc_core = tcase_create ("Btree operation");
	tcase_set_timeout(tc_core, 3); //timeout = 1s
	tcase_add_checked_fixture(tc_core, setup_listener, teardown_listener);
	tcase_add_test (tc_core, setup_teardown);
	tcase_add_test (tc_core, unique);
	tcase_add_test (tc_core, index_and_create);
	tcase_add_test (tc_core, create_and_index);
	tcase_add_test (tc_core, content_possibilities);
	tcase_add_test (tc_core, isolation);
	tcase_add_test (tc_core, modify);
	tcase_add_test (tc_core, listing_index);
	tcase_add_test (tc_core, add_rollback);
	tcase_add_test (tc_core, multi);
	tcase_add_test (tc_core, visibility_map);
	tcase_add_test (tc_core, root_remove);
	tcase_add_test (tc_core, subqueries_calculation);
	tcase_add_test (tc_core, horizonMove);
	
   // tcase_add_test (tc_core, loop);
	//tcase_add_test_raise_signal(tc_core, segfault, SIGSEGV);
	
	suite_add_tcase (s, tc_no_startup);
    suite_add_tcase (s, tc_core);
	
	return s;
}
