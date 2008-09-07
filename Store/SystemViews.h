#ifndef __STORE_SYSTEMVIEWS_H__
#define __STORE_SYSTEMVIEWS_H__

namespace Store
{
	class SystemViews;
	class SystemView;
	class InformationView;
	class AllViewsView;
	class CounterView;
};

#include "Struct.h"
#include "File.h"
#include "Buffer.h"
#include "PagePointer.h"
#include "Log/Logs.h"
#include "DBLogicalID.h"
#include "DBDataValue.h"
#include "DBObjectPointer.h"
#include "Store.h"
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <map>
#include <SystemStats/AllStats.h>
#include <SystemStats/SystemStats.h>


#ifdef DEBUG_MODE
#include "Errors/ErrorConsole.h"
#endif

using namespace std;
using namespace SystemStatsLib;

namespace Store
{
	class SystemViews
	{
	protected:
	#ifdef DEBUG_MODE
		ErrorConsole* ec;
	#endif
		unsigned int tableOfUsedId[0x80000]; // 2MB table of used logicall id
		unsigned int usedIdCount;
		unsigned int currentId;

		ObjectPointer* emptyObject;
	public:
		map<string, SystemView*> mapOfViews;

		SystemViews();
		virtual ~SystemViews();

		vector<int>* getItems(TransactionID* tid);
		vector<int>* getItems(TransactionID* tid, const char* name);

		virtual int getObject(TransactionID* tid, LogicalID* lid, AccessMode mode, ObjectPointer*& object);

		int createNextId(LogicalID*& id);
		void releaseID(LogicalID* id);

		void registerView(string name, SystemView* view);
	};

	class SystemView
	{
		protected:
			SystemViews* systemViews;
		public:
			SystemView() {}
			virtual ~SystemView() {};
			virtual void init(SystemViews* views);
			virtual int getObject(TransactionID* tid, LogicalID* lid, AccessMode mode, ObjectPointer*& object) {return -1; } ;
			virtual void refresh(ObjectPointer*& object) {};

			void createObjectPointer(const char* name, DataValue* value, ObjectPointer*& p);
	};

	class InformationView: public SystemView
	{
		private:
			ObjectPointer* bag;
			ObjectPointer* databaseName;
			ObjectPointer* databaseVersion;

			DataValue* dbNameValue;
			DataValue* dbVerValue;
			DataValue* bagValue;
		public:
			InformationView() {};
			virtual ~InformationView();
			void init(SystemViews* views);
			int getObject(TransactionID* tid, LogicalID* lid, AccessMode mode, ObjectPointer*& object);
			void refresh(ObjectPointer*& object);
	};

	class AllViewsView: public SystemView
	{
		private:
			ObjectPointer* bag;
			vector<ObjectPointer*>* viewsName;
		public:
			AllViewsView() {};
			virtual ~AllViewsView();
			void init(SystemViews* views);
			int getObject(TransactionID* tid, LogicalID* lid, AccessMode mode, ObjectPointer*& object);
			void refresh(ObjectPointer*& object);
	};

	class CounterView: public SystemView
	{
		private:
			ObjectPointer* count;
			int c;
		public:
			CounterView() {};
			virtual ~CounterView();
			void init(SystemViews* views);
			int getObject(TransactionID* tid, LogicalID* lid, AccessMode mode, ObjectPointer*& object);
			void refresh(ObjectPointer*& object);
	};

	class SystemStatsView: public SystemView
		{
			private:
				ObjectPointer* bag;
				vector<ObjectPointer*>* viewsName;
				string name;
			public:
				SystemStatsView(string name) {this->name = name;};
				virtual ~SystemStatsView();
				ObjectPointer* createObjectFromSystemStats(SystemStats* ss);
				void init(SystemViews* views);
				int getObject(TransactionID* tid, LogicalID* lid, AccessMode mode, ObjectPointer*& object);
				void refresh(ObjectPointer*& object);
		};
};

#endif // __STORE_SYSTEMVIEWS_H__
