#include <iostream>

#include <unistd.h>

#include "DBStoreManager.h"
#include "Store.h"
#include "Map.h"
#include "DBLogicalID.h"
#include "../Config/SBQLConfig.h"
#include "../Log/Logs.h"
#include "../TransactionManager/Transaction.h"
#include "Errors/ErrorConsole.h"

using namespace std;
using namespace Store;
using namespace Errors;

void dump_roots(NamedRoots* roots, int t_id, int t_timestamp)
{
	cout << "   getRoots(" << t_id << ", " << t_timestamp << ")\n";
	vector<int>* rv = roots->getRoots(t_id, t_timestamp);

	cout << "   = {";
	if (rv)
	{
		for (vector<int>::iterator ri = rv->begin(); ri != rv->end(); ri++)
			cout << " " << (int) *ri;
		delete rv;
	}
	cout << " }\n";
}

void dump_roots(NamedRoots* roots, char* name, int t_id, int t_timestamp)
{
	cout << "   getRoots(\"" << name << "\", " << t_id << ", " << t_timestamp << ")\n";
	vector<int>* rv = roots->getRoots(name, t_id, t_timestamp);

	cout << "   = {";
	if (rv)
	{
		for (vector<int>::iterator ri = rv->begin(); ri != rv->end(); ri++)
			cout << " " << (int) *ri;
		delete rv;
	}
	cout << " }\n";
}

int main(int argc, char* argv[])
{
	SBQLConfig* config = new SBQLConfig("Store");
	config->init("example.conf");
	ErrorConsole* ec = new ErrorConsole("Store");
	ec->init(1);
	LogManager* log = new LogManager();
	log->init();

	if (argc == 1)
	{
		DBStoreManager* store = new DBStoreManager();
		store->init(log);
		store->setTManager(TransactionManager::getHandle());
		store->start();
		cout << "Store started" << endl;
		
		TransactionID tid(1);
		cout << "TID ok" << endl;
/*		DataValue* dv = store->createIntValue(7);
		cout << "Created INT" << endl;
		ObjectPointer* op_kilof;
	//	ObjectPointer* op_motyka;
		
		cout << "Odpalam createObject" << endl;
		store->createObject(&tid, "kilof", dv, op_kilof);

		store->addRoot(&tid, op_kilof);
		

	//	dv = store->createStringValue("ala ma kota");
	//	cout << "Created String" << endl;

	//	store->createObject(&tid, "motyka", dv, op_motyka);
		
		LogicalID* lid =op_kilof->getLogicalID()->clone();
		cout << lid->toString() << endl;
		delete op_kilof;
		op_kilof = NULL;
		
		cout << "Odpalam getObject" << endl;
		cout << lid->toString() << endl;
		store->getObject(&tid, lid, Store::Write, op_kilof);
		cout << op_kilof->toString() << endl;
		
		store->addRoot(&tid, op_motyka);


		lid = op_motyka->getLogicalID();
		op_motyka = NULL;
		
		cout << "Odpalam getObject" << endl;
		store->getObject(&tid, lid, Store::Write, op_motyka);
		cout << op_motyka->toString() << endl;
		
		
		vector<ObjectPointer*>* vec;
		cout << "Odpalam getRoots(ALL)" << endl;
		store->getRoots(&tid, vec);
		
		vector<ObjectPointer*>::iterator oi;
		for(oi=vec->begin(); oi!=vec->end(); oi++)
			cout << (*oi)->toString() << endl;
		
		
		cout << "Odpalam deleteObject" << endl;
		store->deleteObject(&tid, *(vec->begin()));
		store->removeRoot(&tid, *(vec->begin()));
		
*/

		cout << "Odpalam obiekty zlozone" << endl;
		ObjectPointer* op_woda;

		DataValue* dv = store->createDoubleValue(3.7);
		cout << dv->toString() << endl;
		store->createObject(&tid, "woda", dv, op_woda);
		delete dv;
		cout << op_woda->toString() << endl;

		ObjectPointer* op_wiadro;
		vector<LogicalID*>* lvec = new vector<LogicalID*>(0);
//		lvec->push_back(op_woda->getLogicalID()/*->clone()*/);
		dv = store->createVectorValue(lvec);
		delete lvec;
		cout << dv->toString() << endl;
		store->createObject(&tid, "wiadro", dv, op_wiadro);
		delete dv;
		cout << op_wiadro->toString() << endl;
		
		store->addRoot(&tid, op_wiadro);

		lvec = new vector<LogicalID*>(0);
		lvec->push_back(op_woda->getLogicalID()/*->clone()*/);
		dv = store->createVectorValue(lvec);
		delete lvec;
		store->modifyObject(&tid, op_wiadro, dv);
		//delete op_wiadro;
		delete dv;
		cout << op_wiadro->toString() << endl;

		vector<ObjectPointer*>* ov;
		store->getRoots(&tid, ov);
		cout << ov->size() << endl;
		store->removeRoot(&tid, (*ov)[0]);
		store->getRoots(&tid, ov);
		cout << ov->size() << endl;

		cout << "Getting roots:\n";
		vector<int>* rv = store->getRoots()->getRoots(0, 123456789);
		if (rv)
		{
			vector<int>::iterator ri;
			for (ri = rv->begin(); ri != rv->end(); ri++)
				cout << (int) *ri << "\n";
			delete rv;
		}


		delete op_woda;
//		delete op_wiadro;
				
/*		LogicalID* lid = op_wiadro->getLogicalID()->clone();
		delete op_wiadro; // bez clone, lid wskazywalby teraz na usuniety lid
		cout << lid->toString() << endl;
		op_wiadro = NULL;

		store->getObject(&tid, lid, Store::Write, op_wiadro);
		cout << op_wiadro->toString() << endl;

		dv = op_wiadro->getValue();
		lvec = dv->getVector();
		cout << (*lvec)[0]->toString() << endl;

		store->getObject(&tid, (*lvec)[0], Store::Write, op_woda);
		cout << op_woda->toString() << endl;
*/		
//		vector<LogicalID*>* lvec = dv->getVector();
/*		lvec->push_back(op_woda->getLogicalID());
cout << "DZIWNY KOD ---------\n";
		cout << op_woda->getLogicalID()->toInteger() << endl;
		*dv = *lvec;
		cout << op_woda->getLogicalID()->toInteger() << endl;
		
		cout << "USTAWIAM OP VAL\n";
//		op_wiadro->setValue(dv);
		cout << "DONEEEE\n";
		
		store->deleteObject(&tid, op_wiadro);
//		store->createObject(&tid, op_wiadro->getName(), dv, op_wiadro);
		vector<LogicalID*>* xvec = new vector<LogicalID*>(0);
//		cout << op_woda->getLogicalID()->toInteger() << endl;
//		xvec->push_back(op_woda->getLogicalID());
//		dv = store->createVectorValue(xvec);
//		store->createObject(&tid, op_wiadro->getName(), dv, op_wiadro);
*/		
		store->stop();
	//	log->shutdown();
		cout << "END" << endl;
		return 0;
	}

	if (argc > 1 && strcmp(argv[1], "mk") == 0) 
	{
		DBStoreManager* store = new DBStoreManager();
		store->init(log);
		store->setTManager(TransactionManager::getHandle());
		store->start();
		PagePointer* p[1000];

		for (int i = 0; i < 1000; i++) {
			p[i] = store->getBuffer()->getPagePointer(STORE_FILE_DEFAULT, i);
		//	p->aquire();
		//	p->release(0);
		}
		sleep(10);
		store->stop();
		cout << "Po tescie mk" << endl;
	}

	if (argc > 1 && strcmp(argv[1], "mo") == 0)
	{
		DBStoreManager* store = new DBStoreManager();
		store->init(log);
		store->setTManager(TransactionManager::getHandle());
		store->start();

		TransactionID* tid = new TransactionID(0);
		tid->setTimeStamp(log->getLogicalTimerValue());

		int m_i;

		for (m_i = 1; m_i <= 12345; m_i++)
		{
			ObjectPointer* op = 0;
			DataValue* dv = store->createIntValue(m_i);
			store->createObject(tid, "kilof", dv, op);
			store->addRoot(tid, op);
			delete dv;
			delete op;
		}

		vector<ObjectPointer*>* vec;
		store->getRoots(tid, "kilof", vec);

		vector<ObjectPointer*>::iterator oi;
		for (oi = vec->begin(); oi != vec->end(); oi++)
			cout << (*oi)->toString() << endl;

/*
		vector<int>* rv = store->getRoots()->getRoots("kilof", tid->getId(), tid->getTimeStamp());

		if (rv)
		{
			for (vector<int>::iterator ri = rv->begin(); ri != rv->end(); ri++)
			{
				DBLogicalID* lid = new DBLogicalID((int) *ri);
				ObjectPointer* op = 0;
				store->getObject(tid, lid, Store::Read, op);
				cout << op->toString() << endl;
				delete op;
				delete lid;
			}

			delete rv;
		}

		NamedRoots* roots = store->getRoots();

		roots->addRoot(256, "aaa", 1, 123456789);
		roots->addRoot(257, "aaa", 1, 123456789);
		roots->commitTransaction(1);

		for (int m = 0; m < 256; m++)
			roots->addRoot(m, "ccc", 2, 123456789);

		dump_roots(roots, 2, 123456794);

		for (int m = 16; m < 128; m++)
			roots->removeRoot(m, 2, 123456789);
		dump_roots(roots, 2, 123456789);

		roots->abortTransaction(2);

		dump_roots(roots, 3, 123456789);
*/

		store->stop();

		string aaaa;
		cin >> aaaa;
	}
	else
	{
		DBStoreManager* store = new DBStoreManager();
		store->init(log);
		store->setTManager(TransactionManager::getHandle());
		store->start();
	
		LogicalID* lid = new DBLogicalID(175);
		cout << lid->toString() << endl;

		DataValue* value = store->createIntValue(7);
		ObjectPointer* object = NULL;
		store->createObject(0, "kilof", value, object);
		if (object)
			cout << object->toString() << "\n";
		else
			cout << "Obiektu nie utworzono\n";
		return 0;

		vector<ObjectPointer*>* objects = NULL;
		store->getRoots(0, objects);
		vector<ObjectPointer*>::iterator iterator;

		for (iterator = objects->begin(); iterator != objects->end(); iterator++)
			cout << ((ObjectPointer*) *iterator)->toString() << "\n";
		
		store->stop();
/*
	DataValue* value = store->createIntValue(7);
	ObjectPointer* object = NULL;
	store->createObject(0, "kilof", value, object);
	store->addRoot(0, object);

	vector<ObjectPointer*>* objects = NULL;
	store->getRoots(0, objects);
	vector<ObjectPointer*>::iterator iterator;

	for (iterator = objects->begin(); iterator != objects->end(); iterator++)
	{
		ObjectPointer* o = (ObjectPointer*) *iterator;

		if (o->getName() == "kilof")
			cout << o->toString() << "\n";
	}

////////////////////////////////////////

	vector<ObjectPointer*>* objects = NULL;
	store->getRoots(0, "kilof", objects);
	vector<ObjectPointer*>::iterator iterator;

	for (iterator = objects->begin(); iterator != objects->end(); iterator++)
	{
		ObjectPointer* o = (ObjectPointer*) *iterator;
		cout << o->toString() << "\n";
	}

*/
	}

	return 0;
}
