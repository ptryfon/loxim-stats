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

#include <Store/Struct.h>
#include <Store/File.h>
#include <Store/Buffer.h>
#include <Store/PagePointer.h>
#include <Log/Logs.h>
#include <Store/DBLogicalID.h>
#include <Store/DBDataValue.h>
#include <Store/DBObjectPointer.h>
#include <Store/Store.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <map>
#include <SystemStats/Statistics.h>

#ifdef DEBUG_MODE
#include <Errors/ErrorConsole.h>
#endif

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

		ObjectPointer *emptyObject;
	public:
		std::vector<SystemView*> views;

		SystemViews();
		virtual ~SystemViews();

		vector<int>* get_items(Transaction *tr) const;
		vector<int>* get_items(Transaction *tr, string name) const;

		virtual int get_object(TransactionID *tid, LogicalID *lid,
			AccessMode mode, ObjectPointer *&object) const;

		unsigned int convert_name(const std::string &name) const;
		void create_next_id(LogicalID *&id);
		void release_id(LogicalID *id);
	};

	class SystemView
	{
		protected:
			SystemViews *systemViews;
			ObjectPointer *bag;
		public:
			SystemView(SystemViews *views);
			virtual ~SystemView();
			virtual int get_object(TransactionID */*tid*/, LogicalID */*lid*/,
				AccessMode /*mode*/, ObjectPointer *&/*object*/) const = 0;
			virtual void refresh(Transaction */*tr*/, ObjectPointer *&/*object*/);

			void create_object_pointer(const std::string &name, DataValue *value, ObjectPointer *&p);
	};

	class InformationView: public SystemView
	{
		private:
			ObjectPointer* databaseName;
			ObjectPointer* databaseVersion;

			DataValue* dbNameValue;
			DataValue* dbVerValue;
			DataValue* bagValue;
		public:
			InformationView(SystemViews *views);
			~InformationView();
			int get_object(TransactionID *tid, LogicalID *lid,
				AccessMode mode, ObjectPointer *&object) const;
	};

	class AllViewsView: public SystemView
	{
		private:
			std::vector<ObjectPointer*>* viewsName;
		public:
			AllViewsView(SystemViews *views);
			~AllViewsView();
			int get_object(TransactionID *tid, LogicalID *lid,
				AccessMode mode, ObjectPointer *&object) const;
	};

	class CounterView: public SystemView
	{
		private:
			int c;
		public:
			CounterView(SystemViews *views);
			int get_object(TransactionID *tid, LogicalID *lid,
				AccessMode mode, ObjectPointer *&object) const;
			void refresh(Transaction *tr, ObjectPointer *&object);
	};

	class SystemStatsView: public SystemView
		{
			private:
				std::vector<ObjectPointer*>* viewsName;
				std::string name;
			public:
				SystemStatsView(const std::string &name, SystemViews *views);
				~SystemStatsView();
				ObjectPointer* create_object_from_abstract_stats(SystemStatsLib::AbstractStats *as);
				int get_object(TransactionID *tid, LogicalID *lid,
					AccessMode mode, ObjectPointer *&object) const;
				void refresh(Transaction *tr, ObjectPointer *&object);
		};

	class RootsView: public SystemView {
	private:
		DataValue *bagValue;
		std::vector<ObjectPointer*> objects;

		void fetch(Transaction *tr);
		void release();
	public:
		RootsView(SystemViews *views);
		~RootsView();
		int get_object(TransactionID *tid, LogicalID *lid,
			AccessMode mode, ObjectPointer *&object) const;
		void refresh(Transaction *tr, ObjectPointer *&object);
	};

};

#endif // __STORE_SYSTEMVIEWS_H__
