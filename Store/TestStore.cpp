#include <iostream>
#include "Store.h"
#include "Roots.h"
#include "DBStoreManager.h"
#include "DBLogicalID.h"

using namespace std;
using namespace Store;

int main(int argc, char* argv[])
{
	if (argc > 1 && strcmp(argv[1], "mo") == 0)
	{
		DBStoreManager* store = new DBStoreManager();
		store->init(0, 0);
		store->start();

		Roots* roots = store->getRoots();

		cout << "getLastAssigned() = " << roots->getLastAssigned() << "\n";
		cout << "addRoot(0x30) = " << roots->addRoot(0x30) << "\n";

//		vector<int>* rv = NULL;
//		roots->getRoots();
//		vector<int>::iterator ri;
//		for (ri = rv->begin(); ri != rv->end(); rv++)
//			cout << (int) *ri << "\n";

		store->stop();
	}
	else
	{
		DBStoreManager* store = new DBStoreManager();
		store->init(0, 0);
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
