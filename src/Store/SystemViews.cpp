#include <Store/SystemViews.h>

using namespace std;
using namespace SystemStatsLib;

namespace Store
{
	#define INFORMATION_VIEW 					0
	#define ALL_VIEWS_VIEW 						1
	#define COUNTER_VIEW								2
	#define SESSION_STATS_VIEW				3
	#define STORE_STATS_VIEW					4
	#define CONFIG_STATS_VIEW					5
	#define TRANSACTIONS_STATS_VIEW	6
	#define QUERIES_STATS_VIEW				7
	#define ROOTS_VIEW									8
	#define NO_VIEW											9

	#define FILL_VIEW(values_vec, names_vec, set_fun) {	\
		ObjectPointer *object;																\
		DataValue *db_name_value = new DBDataValue();		\
		db_name_value->set_fun(output->values_vec[i]);			\
		create_object_pointer(output->names_vec[i],					\
			db_name_value, object);														\
		if (object) {																					\
			viewsName->push_back(object);										\
			val->push_back(object->getLogicalID());						\
		}																										\
	}

	SystemViews::SystemViews()
	{
#ifdef DEBUG_MODE
		this->ec = new ErrorConsole("Store");
		*this->ec << "Store::SystemViews::create done";
#endif
		usedIdCount = 0;
		currentId = 0;

		LogicalID *id;
		try {
			create_next_id(id);
			DataValue* emptyValue = new DBDataValue("?");
			emptyObject = new DBObjectPointer(string("EmptyObject"), emptyValue, id);
			emptyObject->setIsRoot(0);
		} catch (...) {
#ifdef DEBUG_MODE
			*this->ec << "Store::SystemViews::Can't create LID for EmptyObject!";
#endif
		}

		views.push_back(new InformationView(this));
		views.push_back(new AllViewsView(this));
		views.push_back(new CounterView(this));
		views.push_back(new SystemStatsView("SESSIONS_STATS", this));
		views.push_back(new SystemStatsView("STORE_STATS", this));
		views.push_back(new SystemStatsView("CONFIG_STATS", this));
		views.push_back(new SystemStatsView("TRANSACTIONS_STATS", this));
		views.push_back(new SystemStatsView("QUERIES_STATS", this));
		views.push_back(new RootsView(this));
	}

	SystemViews::~SystemViews()
	{
#ifdef DEBUG_MODE
		*this->ec << "Store::SystemViews::finalize done";
#endif
		if (emptyObject) {
			delete emptyObject;
		}

		for (unsigned int i = 0; i < views.size(); ++i)
			delete views[i];
	}

	vector<int>* SystemViews::get_items(Transaction *tr) const
	{
		//cout << "===> SystemViews::getItems(" << tid->getId() << ")" << endl;
		vector<int>* systemviews = new vector<int>;

		ObjectPointer *object;

		for (unsigned int i = 0; i < views.size(); ++i) {
			views[i]->refresh(tr, object);
			systemviews->push_back(object->getLogicalID()->toInteger());
		}

		return systemviews;
	};

	vector<int>* SystemViews::get_items(Transaction *tr, string name) const
	{
#ifdef DEBUG_MODE
		*this->ec << "Store::SystemViews::getItems";
#endif

		unsigned int index = convert_name(name);

		vector<int>* systemviews = new vector<int>();
		ObjectPointer *object;

		if (index < views.size()) {
			views[index]->refresh(tr, object);
			systemviews->push_back(object->getLogicalID()->toInteger());
		}

		return systemviews;
	}

	unsigned int SystemViews::convert_name(const string &name) const {
		if (!name.compare("%Information"))
			return INFORMATION_VIEW;
		if (!name.compare("%AllViews"))
			return ALL_VIEWS_VIEW;
		if (!name.compare("%COunter"))
			return COUNTER_VIEW;
		if (!name.compare("%Sessions"))
			return SESSION_STATS_VIEW;
		if (!name.compare("%Store"))
			return STORE_STATS_VIEW;
		if (!name.compare("%Configs"))
			return CONFIG_STATS_VIEW;
		if (!name.compare("%Transactions"))
			return TRANSACTIONS_STATS_VIEW;
		if (!name.compare("%Queries"))
			return QUERIES_STATS_VIEW;
		if (!name.compare("Roots"))
			return ROOTS_VIEW;
		return NO_VIEW;
	}

	int SystemViews::get_object(TransactionID *tid, LogicalID *lid,
		AccessMode mode, ObjectPointer *&object) const
	{
		//cout << "===> SystemViews::getObject(" << tid->getId() << ", " << lid->toInteger() << ")" << endl;
		ObjectPointer *found_object;

		for (unsigned int i = 0; i < views.size(); ++i)
			if (views[i]->get_object(tid, lid, mode, found_object) == 0) {
				if (found_object)
					object = found_object;
				return 0;
			}

		/* Id Object is still null then put empty object */
		object = emptyObject;
		return 0;
	}

	void SystemViews::create_next_id(LogicalID *&id) {
		if (usedIdCount >= 0xFFFFFF) {
			// All id's used no free available
			throw (-2);
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
						return;
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

	void SystemViews::release_id(LogicalID* id) {
		unsigned int idint = id->toInteger();
		unsigned int tabId = idint >> 5; // divide 32
		unsigned int tab = tableOfUsedId[tabId];
		unsigned int currentBit = idint & 0x20;
		unsigned int bitmask = 1 << currentBit;
		/* of bit table */
		tableOfUsedId[tabId] = tab & (~bitmask);
		usedIdCount--;
	}


	SystemView::SystemView(SystemViews *views) : systemViews(views), bag(0) {}

	SystemView::~SystemView() {
		if(bag)
			delete bag;
	}

	void SystemView::create_object_pointer(const string &name, DataValue *value, ObjectPointer *&p) {
		LogicalID* id;

		try {
			systemViews->create_next_id(id);
			p = new DBObjectPointer(name, value, id);
			p->setIsRoot(0);
		} catch (...) {
		}
	}

	void SystemView::refresh(Transaction */*tr*/, ObjectPointer *&object) {
		object = bag;
	}

	/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
	/* InformationView */

	InformationView::InformationView(SystemViews *views) : SystemView(views) {
		dbNameValue = new DBDataValue();
		dbNameValue->setString("Loxim");

		dbVerValue = new DBDataValue();
		dbVerValue->setString("1.0.0.0");

		create_object_pointer("Name", dbNameValue, databaseName);
		create_object_pointer("Version", dbVerValue, databaseVersion);

		bagValue = new DBDataValue();

		vector<LogicalID*>* val = new vector<LogicalID*>();
		if (databaseName) val->push_back(databaseName->getLogicalID());
		if (databaseVersion) val->push_back(databaseVersion->getLogicalID());

		bagValue->setVector(val);

		create_object_pointer("Bag", bagValue, this->bag);
	}

	InformationView::~InformationView() {
		if (databaseName)
			delete databaseName;
		if (databaseVersion)
			delete databaseVersion;
	}

	int InformationView::get_object(TransactionID */*tid*/, LogicalID *lid,
		AccessMode /*mode*/, ObjectPointer *&object) const {

		if (this->bag && (lid->toInteger() == this->bag->getLogicalID()->toInteger()))
			object = this->bag;
		else if (databaseName && (lid->toInteger() == databaseName->getLogicalID()->toInteger()))
			object = databaseName;
		else if (databaseVersion && (lid->toInteger() == databaseVersion->getLogicalID()->toInteger()))
			object = databaseVersion;
		else {
			return -1;
		}
		return 0;
	}

	/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
	/* AllViewsView */

	AllViewsView::AllViewsView(SystemViews *views) : SystemView(views) {
		vector<LogicalID*>* val = new vector<LogicalID*>();

		viewsName = new vector<ObjectPointer*>();

		for (unsigned int i = 0; i < views->views.size(); ++i) {
			DataValue* dbNameValue = new DBDataValue();
			dbNameValue->setInt(i);

			ObjectPointer *object;
			create_object_pointer("View", dbNameValue, object);

			if (object) {
				viewsName->push_back(object);
				val->push_back(object->getLogicalID());
			}
		}

		DataValue* bagValue = new DBDataValue();
		bagValue->setVector(val);
		create_object_pointer("Bag", bagValue, this->bag);
	}

	AllViewsView::~AllViewsView() {
		if (viewsName) {
			for (unsigned int i = 0; i < viewsName->size(); i++) {
				delete (*viewsName)[i];
			}
			delete viewsName;
		}
	}

	int AllViewsView::get_object(TransactionID */*tid*/, LogicalID *lid,
		AccessMode /*mode*/, ObjectPointer *&object) const {

		if (this->bag && (lid->toInteger() == this->bag->getLogicalID()->toInteger()))
			object = this->bag;
		else {
			for (unsigned int i=0; i<viewsName->size(); i++) {
				if ((*viewsName)[i]->getLogicalID()->toInteger() == lid->toInteger()) {
					object = (*viewsName)[i];
					return 0;
				}
			}
			return -1;
		}
		return 0;
	}

	/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
	/* CounterView */

	CounterView::CounterView(SystemViews *views) : SystemView(views) {
		c = 0;

		DataValue* dbCount = new DBDataValue();
		dbCount->setInt(c);

		create_object_pointer("Count", dbCount, this->bag);
	}

	int CounterView::get_object(TransactionID* /*tid*/, LogicalID* lid,
		AccessMode /*mode*/, ObjectPointer*& object) const {

		if (this->bag && (lid->toInteger() == this->bag->getLogicalID()->toInteger()))
			object = this->bag;
		else {
			return -1;
		}
		return 0;
	}

	void CounterView::refresh(Transaction */*tr*/, ObjectPointer *&object) {
		this->bag->getValue()->setInt(c++);
		object = this->bag;
	}

	/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
	/* SystemStatsView */

	SystemStatsView::SystemStatsView(const string &name, SystemViews *views) :
	SystemView(views), name(name) {

		AbstractStats *as = Statistics::get_unified_statistics(pthread_self(), name);

		viewsName = new vector<ObjectPointer*>();
		bag = create_object_from_abstract_stats(as);
	}

	SystemStatsView::~SystemStatsView() {
		if (viewsName) {
			for (unsigned int i = 0; i < viewsName->size(); i++) {
				delete (*viewsName)[i];
			}
			delete viewsName;
		}
	}

	ObjectPointer* SystemStatsView::create_object_from_abstract_stats(AbstractStats *as) {

		vector<LogicalID*> *val = new vector<LogicalID*>();
		StatsOutput *output = as->get_stats_output();

		for (unsigned int i = 0; i < output->int_stats.size(); ++i)
			FILL_VIEW(int_stats, int_names, setInt)

		for (unsigned int i = 0; i < output->double_stats.size(); ++i)
			FILL_VIEW(double_stats, double_names, setDouble)

		for (unsigned int i = 0; i < output->string_stats.size(); ++i)
			FILL_VIEW(string_stats, string_names, setString)

		delete output;
		DataValue* bagValue = new DBDataValue();
		bagValue->setVector(val);
		ObjectPointer* res;
		create_object_pointer(as->get_name().c_str(), bagValue, res);
		if (as)
			delete as;
		return res;
	}

	int SystemStatsView::get_object(TransactionID */*tid*/, LogicalID *lid,
	AccessMode /*mode*/, ObjectPointer *&object) const {

		if (this->bag && (lid->toInteger() == this->bag->getLogicalID()->toInteger())) {
			//cout << "    ===> SystemStatsViews::getObject(" << tid->getId() << ", " << lid->toInteger() << ") = BAG" << endl;
			object = this->bag;
		}
		else {
			for (unsigned int i=0; i<viewsName->size(); i++) {
				if ((*viewsName)[i]->getLogicalID()->toInteger() == lid->toInteger()) {
					//cout << "    ===> SystemStatsViews::getObject(" << tid->getId() << ", " << lid->toInteger() << ") = FOUND" << endl;
					object = (*viewsName)[i];
					return 0;
				}
			}
			//cout << "    ===> SystemStatsViews::getObject(" << tid->getId() << ", " << lid->toInteger() << ") = NOT FOUND !!!" << endl;
			return -1;
		}
		return 0;
	}

	void SystemStatsView::refresh(Transaction */*tr*/, ObjectPointer *&object) {
		if (viewsName) {
			for (unsigned int i = 0; i < viewsName->size(); i++) {
				delete (*viewsName)[i];
			}
			delete viewsName;
		}
		delete this->bag;

		AbstractStats *as = Statistics::get_unified_statistics(pthread_self(), name);

		viewsName = new vector<ObjectPointer*>();
		this->bag = create_object_from_abstract_stats(as);
		object = this->bag;
	}

	/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
	/* RootsView */

	RootsView::RootsView(SystemViews *views) : SystemView(views) {\
		bagValue = new DBDataValue();
		vector<LogicalID*>* val = new vector<LogicalID*>();
		bagValue->setVector(val);
		create_object_pointer("Bag", bagValue, this->bag);
//std::cerr << " + bag LID: " << bag->getLogicalID() << std::endl;
	}

	RootsView::~RootsView() {
		release();
	}

	int RootsView::get_object(TransactionID */*tid*/, LogicalID *lid,
		AccessMode /*mode*/, ObjectPointer *&object) const {

//std::cerr << " + %Roots getObj lid: " << lid->toInteger() << std::endl;
		if (this->bag != NULL && *lid == *this->bag->getLogicalID()) {
			object = this->bag;
			return 0;
		}
		vector<ObjectPointer*>::iterator it;
		for (unsigned int i = 0; i < objects.size(); ++i)
			if (*lid == *(objects[i]->getLogicalID())) {
				object = objects[i];
				return 0;
			}
		return -1;
	}

	void RootsView::refresh(Transaction *tr, ObjectPointer*& object)
	{
//std::cerr << " + %Roots refr" << std::endl;
		release();
		fetch(tr);
		object = this->bag;
	}

	void RootsView::fetch(Transaction *tr)
	{
		bagValue = new DBDataValue();
		vector<LogicalID*>* bagVector = new vector<LogicalID*>();

		vector<ObjectPointer*> *vroots = NULL;
		int res = tr->getRoots(vroots);
//std::cerr << " + res: " << res << "; vroots: " << vroots << std::endl;
		if (res == 0 && vroots != NULL) {
			ObjectPointer *cur;
			DataValue *curValue;

//std::cerr << " + roots count: " << vroots->size() << std::endl;
			vector<ObjectPointer*>::iterator it;
			for (it = vroots->begin(); it != vroots->end(); ++it) {
//std::cerr << " + root obj lid: " << (*it)->getLogicalID()->toInteger() << std::endl;
//std::cerr << " + root obj type: " << (*it)->getValue()->getType() << std::endl;
//std::cerr << " + root obj name: " << (*it)->getName() << std::endl;
				if (false) //TODO: skip system-internal roots
					continue;
				curValue = new DBDataValue();
				curValue->setPointer((*it)->getLogicalID());
				create_object_pointer((*it)->getName().c_str(), curValue, cur);
//std::cerr << " + cur LID: " << cur->getLogicalID()->toInteger() << std::endl;
				objects.push_back(cur);
				bagVector->push_back(cur->getLogicalID());
			}
			delete vroots;
		}

		bagValue->setVector(bagVector);
		create_object_pointer("Bag", bagValue, this->bag);
//std::cerr << " + bag LID: " << bag->getLogicalID()->toInteger() << std::endl;
	}

	void RootsView::release()
	{
		if (this->bag)
			return;
		vector<ObjectPointer*>::iterator it;
		for (it = objects.begin(); it != objects.end(); ++it)
			delete *it;
		delete this->bag;
		this->bag = NULL;
		objects.clear();
	}

};

