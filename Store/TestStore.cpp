#include <iostream>
#include "Store.h"
#include "DBStoreManager.h"

using namespace std;
using namespace Store;

int main(int argc, char* argv[])
{
	DBStoreManager* store = new DBStoreManager();
	store->init(0, 0);
	store->start();

	vector<ObjectPointer*>* objects = 0;
	store->getRoots(0, &objects);
	vector<ObjectPointer*>::iterator iterator;

	for (iterator = objects->begin(); iterator != objects->end(); iterator++)
		cout << ((ObjectPointer*) *iterator)->toString() << "\n";

	store->stop();

	return 0;
}
