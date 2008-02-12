#include "SystemViews.h"

namespace Store
{
	SystemViews::SystemViews()
	{
		#ifdef DEBUG_MODE
				this->ec = new ErrorConsole("Store");
				*this->ec << "Store::SystemViews::create done";
		#endif
		usedIdCount = 0;
		currentId = 0;
		
		LogicalID* id;
		int res = createNextId(id);
		if (res == 0) {
			DataValue* emptyValue = new DBDataValue("?");
			emptyObject = new DBObjectPointer(string("EmptyObject"), emptyValue, id);
			emptyObject->setIsRoot(0);
		} else {
		#ifdef DEBUG_MODE
				*this->ec << "Store::SystemViews::Can't create LID for EmptyObject!";
		#endif
		}
		
		/* In this section of code we are registering
		 * all system views
		 */
		registerView("%Information", new InformationView());
		registerView("%AllViews", new AllViewsView());
		registerView("%Counter", new CounterView());
		
		/* Init views */
		map<const char*,SystemView*>::iterator iter;   
  		for( iter = mapOfViews.begin(); iter != mapOfViews.end(); iter++ ) {
			iter->second->init(this);
		}
	}

	SystemViews::~SystemViews()
	{
		#ifdef DEBUG_MODE
				*this->ec << "Store::SystemViews::finalize done";
				delete(this->ec);
		#endif	
		if (emptyObject) {
			delete emptyObject;
		}
	}
	
	vector<int>* SystemViews::getItems(int transactionID, int transactionTimeStamp)
	{
		vector<int>* systemviews = new vector<int>();

		ObjectPointer* object;
		map<const char*,SystemView*>::iterator iter;   
  		for( iter = mapOfViews.begin(); iter != mapOfViews.end(); iter++ ) {
    		iter->second->refresh(object);
			if (object) {
				#ifdef DEBUG_MODE
						*this->ec << "Store::SystemViews::getItems - add view " << (iter->first) << " [" << (object->getLogicalID()->toInteger()) << "]";
				#endif	
				systemviews->push_back(object->getLogicalID()->toInteger());		
			}
  		}		
	
		return systemviews;
	};
	
	vector<int>* SystemViews::getItems(const char* name, int transactionID, int transactionTimeStamp)
	{
	#ifdef DEBUG_MODE
				*this->ec << "Store::SystemViews::getItems";
		#endif	
		
		vector<int>* systemviews = new vector<int>();

			if (mapOfViews[name]) {
#ifdef DEBUG_MODE
				*this->ec << "Store::SystemViews::mapOfViews[" << name << "]";
#endif
			ObjectPointer* object;
			mapOfViews[name]->refresh(object);
			if (object) {
#ifdef DEBUG_MODE
				*this->ec << "Store::SystemViews::mapOfViews - OBJECT";
#endif
				systemviews->push_back(object->getLogicalID()->toInteger());
			}
		}
		return systemviews;
	}

	int SystemViews::getObject(TransactionID* tid, LogicalID* lid, AccessMode mode, ObjectPointer*& object)
	{
		ObjectPointer* foundObject;
		map<char const*,SystemView*>::iterator iter;   
  		for( iter = mapOfViews.begin(); iter != mapOfViews.end(); iter++ ) {
    		iter->second->getObject(tid, lid, mode, foundObject);
			if (iter->second->getObject(tid, lid, mode, foundObject) == 0) {
				if (foundObject) {
					object = foundObject;
					return 0;
				}
			}
  		}		
		/* Id Object is still null then put empty object */
		object = emptyObject;
		return 0;	
	}	
	
	int SystemViews::createNextId(LogicalID*& id) {
			if (usedIdCount >= 0xFFFFFF) {
			// All id's used no free available
			return -2;
		}
		unsigned int tabId = currentId >> 5; // divide 32
		unsigned int tab;
		
		while (true) {
			tab = tableOfUsedId[tabId];
			if (~(tab ^ 0xFFFFFFFF)) { // Is full
				if (currentId & 0x20) { // if mod 32 > 0
					currentId = tabId << 5; // multiple 32
				}
				currentId+=0x20;
				currentId &= 0x00FFFFFF;
				tabId++;
			} else {
				unsigned int currentBit = currentId & 0xFF;
				unsigned int bitmask = 1 << currentBit;
				while (true) {
					if (~(tab & bitmask)) {
						id = new DBLogicalID((unsigned int)(0xFF000000 + (currentId++)));
						usedIdCount++;
						tableOfUsedId[tabId] = tab | bitmask;	
						return 0;
					}
					currentId++;
					currentBit++;
					bitmask <<= 1;
					if (currentBit & 0xFFFFFFE0) {
						bitmask = 1;
						currentBit = 0;
						currentId -= 0x20;
					}
				}
			}
		}
	}
	
	void SystemViews::releaseID(LogicalID* id) {
		unsigned int idint = id->toInteger();
		unsigned int tabId = idint >> 5; // divide 32
		unsigned int tab = tableOfUsedId[tabId];
		unsigned int currentBit = idint & 0x20;
		unsigned int bitmask = 1 << currentBit;
		/* of bit table */
		tableOfUsedId[tabId] = tab & (~bitmask);
		usedIdCount--;
	}
	
	void SystemViews::registerView(const char* name, SystemView* view) {
		mapOfViews[name] = view;
	}
	
	void SystemView::init(SystemViews* views) {
		systemViews = views;
	}

	void SystemView::createObjectPointer(const char* name, DataValue* value, ObjectPointer*& p) {
		LogicalID* id;
		int res = systemViews->createNextId(id);

		if (res == 0) {
			p = new DBObjectPointer(string(name), value, id);
			p->setIsRoot(0);
		}
	}

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/	
	/* InformationView */

		InformationView::~InformationView() {
		if (databaseName) delete databaseName;
		if (databaseVersion) delete databaseVersion;
		if (bag) delete bag;
	};
	
	void InformationView::init(SystemViews* views) {
		SystemView::init(views);
	
		dbNameValue = new DBDataValue();
		dbNameValue->setString("Loxim");
		
		dbVerValue = new DBDataValue();
		dbVerValue->setString("1.0.0.0");
		
		createObjectPointer("Name", dbNameValue, databaseName);
		createObjectPointer("Version", dbVerValue, databaseVersion);
		
		
		bagValue = new DBDataValue();

		vector<LogicalID*>* val = new vector<LogicalID*>();
		if (databaseName) val->push_back(databaseName->getLogicalID());
		if (databaseVersion) val->push_back(databaseVersion->getLogicalID());
		
		bagValue->setVector(val);

		createObjectPointer("Bag", bagValue, bag);
	}
	
	int InformationView::getObject(TransactionID* tid, LogicalID* lid, AccessMode mode, ObjectPointer*& object) {
		if (bag && (lid->toInteger() == bag->getLogicalID()->toInteger())) object = bag;
		else if (databaseName && (lid->toInteger() == databaseName->getLogicalID()->toInteger())) object = databaseName;
		else if (databaseVersion && (lid->toInteger() == databaseVersion->getLogicalID()->toInteger())) object = databaseVersion;
		else {
			return -1;
		}
		return 0;
	}
	
	void InformationView::refresh(ObjectPointer*& object) {
		object =  bag;
	}
	
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/	
	/* AllViewsView */

	AllViewsView::~AllViewsView() {
		
		if (viewsName) {
			for (int i=0; i<viewsName->size(); i++) {
				delete (*viewsName)[i];
			}
			delete viewsName;
		}
		if (bag) delete bag;
	};
	
	void AllViewsView::init(SystemViews* views) {
		SystemView::init(views);

		vector<LogicalID*>* val = new vector<LogicalID*>();
	
		viewsName = new vector<ObjectPointer*>();
		map<char const*,SystemView*>::iterator iter;   
  		for( iter = views->mapOfViews.begin(); iter != views->mapOfViews.end(); iter++ ) {
			DataValue* dbNameValue = new DBDataValue();
			dbNameValue->setString(iter->first);
			
			ObjectPointer* object;
			createObjectPointer("View", dbNameValue, object);
			
			if (object) {
				viewsName->push_back(object);
				val->push_back(object->getLogicalID());
			}
		}
		
		DataValue* bagValue = new DBDataValue();
		bagValue->setVector(val);
		createObjectPointer("Bag", bagValue, bag);
	}
	
	int AllViewsView::getObject(TransactionID* tid, LogicalID* lid, AccessMode mode, ObjectPointer*& object) {
		if (bag && (lid->toInteger() == bag->getLogicalID()->toInteger())) object = bag;
		else {
			for (int i=0; i<viewsName->size(); i++) {
    			if ((*viewsName)[i]->getLogicalID()->toInteger() == lid->toInteger()) {
					object = (*viewsName)[i];							
					return 0;
				}
			}
			return -1;
		}
		return 0;
	}
	
	void AllViewsView::refresh(ObjectPointer*& object) {
		object =  bag;
	}
	
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/	
	/* CounterView */

	CounterView::~CounterView() {
		if (count) delete count;
	};
	
	void CounterView::init(SystemViews* views) {
		SystemView::init(views);
	
		c = 0;
		
		DataValue* dbCount = new DBDataValue();
		dbCount->setInt(c);
		
		createObjectPointer("Count", dbCount, count);;
	}
	
	int CounterView::getObject(TransactionID* tid, LogicalID* lid, AccessMode mode, ObjectPointer*& object) {
		if (count && (lid->toInteger() == count->getLogicalID()->toInteger())) object = count;
		else {
			return -1;
		}
		return 0;
	}
	
	void CounterView::refresh(ObjectPointer*& object) {
		count->getValue()->setInt(c++);
		object =  count;
	}
		
};
