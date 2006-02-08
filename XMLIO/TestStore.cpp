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

DBStoreManager* store;

void dropObject(TransactionID* tid, ObjectPointer* obj) {
		cout << "<OBJECT name=\'" << obj->getName() << "\'>";
		DataValue* val = obj->getValue();
		switch (val->getType()) {
			case Store::Integer:
				cout << val->getInt(); break;
			case Store::Double:
				cout << val->getDouble(); break;
			case Store::String:
				cout << '"' << val->getString() << '"'; break;
			case Store::Pointer:
				cout << "*" << val->getPointer(); break;
			case Store::Vector: {
				vector<LogicalID*>::iterator pi;
				for(pi=val->getVector()->begin(); pi!=val->getVector()->end(); pi++)
				{
					ObjectPointer* obj;
					store->getObject(tid, (*pi), Read, obj);
					dropObject(tid, obj);
				}			
				} break;
			default:
				cout << "DV ERROR"; break;				
		}
		cout << "</OBJECT>";
}

int main(int argc, char* argv[])
{
	SBQLConfig* config = new SBQLConfig("Store");
	config->init("example.conf");
	ErrorConsole* ec = new ErrorConsole("Store");
	ec->init(1);
	LogManager* log = new LogManager();
	log->init();
	
	store = new DBStoreManager();
	store->init(log);
	store->setTManager(TransactionManager::getHandle());
	store->start();
	cout << "Store started" << endl;	
	TransactionID tid(1);
	cout << "TID ok" << endl;
			
	vector<ObjectPointer*>* vec;
	cout << "Odpalam getRoots(ALL)" << endl;
	store->getRoots(&tid, vec);
	
	vector<ObjectPointer*>::iterator oi;
	for(oi=vec->begin(); oi!=vec->end(); oi++) {
		dropObject(&tid, *oi);
		cout << "\n";		
	}
	delete vec;

	return 0;
}
