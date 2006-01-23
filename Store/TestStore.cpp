#include <iostream>

#include <unistd.h>

#include "Store.h"
#include "Roots.h"
#include "Map.h"
#include "DBStoreManager.h"
#include "DBLogicalID.h"
#include "../Config/SBQLConfig.h"

using namespace std;
using namespace Store;

int main(int argc, char* argv[])
{
	SBQLConfig* config = new SBQLConfig("store");
	config->init("example.conf");

	if (argc > 1 && strcmp(argv[1], "mk") == 0) 
	{
		SBQLConfig* config = new SBQLConfig("store");
		config->init("example.conf");
		DBStoreManager* store = new DBStoreManager();
		store->init(config, 0);
		store->start();
		PagePointer* p;
		PagePointer* p2;

		for (int i = 0; i < 40; i++) {
			p = store->getBuffer()->getPagePointer(STORE_FILE_DEFAULT, i);
			p->aquire();
			if (i != 37)
				p->release();
			else
				p2 = p;
		}
		p = store->getBuffer()->getPagePointer(STORE_FILE_DEFAULT, 50);
		p = store->getBuffer()->getPagePointer(STORE_FILE_DEFAULT, 90);
		sleep(10);
		p2->release();
		store->stop();
		cout << "Po tescie mk" << endl;
	}

	if (argc > 1 && strcmp(argv[1], "mo") == 0)
	{
		DBStoreManager* store = new DBStoreManager();
		store->init(config, 0);
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
		store->init(config, 0);
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
