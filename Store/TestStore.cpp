#include <iostream>

#include <unistd.h>

#include "Store.h"
#include "Roots.h"
#include "Map.h"
#include "DBStoreManager.h"
#include "DBLogicalID.h"
#include "../Config/SBQLConfig.h"
#include "../Log/Logs.h"
#include "../TransactionManager/Transaction.h"
#include "Errors/ErrorConsole.h"

using namespace std;
using namespace Store;
using namespace Errors;

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
//		delete dv;
		cout << op_woda->toString() << endl;

		ObjectPointer* op_wiadro;
		vector<LogicalID*>* lvec = new vector<LogicalID*>(0);
//		lvec->push_back(op_woda->getLogicalID()->clone());
		dv = store->createVectorValue(lvec);
		cout << dv->toString() << endl;
		store->createObject(&tid, "wiadro", dv, op_wiadro);
//		delete dv;
		cout << op_wiadro->toString() << endl;
		
		store->addRoot(&tid, op_wiadro);


		lvec->push_back(op_woda->getLogicalID()/*->clone()*/);
		dv = store->createVectorValue(lvec);
		op_wiadro->setValue(dv);


		
//		delete op_woda;
		
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
		log->shutdown();
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
		//	p->release();
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

		physical_id* pid = NULL;
		Map* map = store->getMap();
		unsigned int lid = map->createLogicalID();
		cout << "createLogicalID() = " << lid << "\n";
		cout << map->getPhysicalID(lid, &pid) << "\n";
		cout << map->setPhysicalID(lid, pid) << "\n";
		if (pid)
			delete pid;
/*
		Roots* roots = store->getRoots();

		cout << "addRoot(0x32) = " << roots->addRoot(0x32) << "\n";
		cout << "addRoot(0x31) = " << roots->addRoot(0x31) << "\n";
		cout << "addRoot(0x30) = " << roots->addRoot(0x30) << "\n";
		cout << "addRoot(0x33) = " << roots->addRoot(0x33) << "\n";
		cout << "removeRoot(0x31) = " << roots->removeRoot(0x31) << "\n";

		cout << "Getting roots:\n";
		vector<int>* rv = roots->getRoots();
		if (rv)
		{
			vector<int>::iterator ri;
			for (ri = rv->begin(); ri != rv->end(); ri++)
				cout << (int) *ri << "\n";
			delete rv;
		}
*/

		store->stop();
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
