#include <iostream>

#include "Store.h"
#include "DBStoreManager.h"

using namespace Store;

int main(int argc, char* argv[])
{
	StoreManager* store = new DBStoreManager();
	vector<ObjectPointer*>* objects = store->getRoots(0);
	vector<ObjectPointer*>::iterator iterator;

	for (iterator = objects->begin(); iterator != objects->end(); iterator++)
		cout << ((ObjectPointer*) *iterator)->toString() << "\n";

	return 0;
}
