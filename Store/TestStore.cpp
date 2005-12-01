#include <iostream>
#include "Store.h"
#include "DBStoreManager.h"
#include "DBLogicalID.h"

using namespace std;
using namespace Store;

int main(int argc, char* argv[])
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

	return 0;
}
