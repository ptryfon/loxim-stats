#ifndef __STORE_MAP_H__
#define __STORE_MAP_H__

namespace Store
{
	class Map;
};

#include "Store.h"
#include "Struct.h"
#include "DBStoreManager.h"
#include "File.h"
#include "PagePointer.h"

#define STORE_MAP_PERPAGE			((STORE_PAGESIZE - sizeof(map_page)) / sizeof(physical_id))
#define STORE_MAP_MAPPAGE(i)		(1 + (i / STORE_MAP_PERPAGE))
#define STORE_MAP_MAPOFFSET(i)		(sizeof(map_page) + sizeof(physical_id) * (i % STORE_MAP_PERPAGE))

using namespace std;

namespace Store
{
	class Map
	{
	private:
		DBStoreManager* store;
		PagePointer* header;

		unsigned int getLastAssigned();
		void setLastAssigned(unsigned int);

	public:
		Map(DBStoreManager* storemgr);
		~Map();

		int initializeFile(File* file);

		unsigned int createLogicalID();
		int getPhysicalID(unsigned int logicalID, physical_id** physicalID);
		int setPhysicalID(unsigned int logicalID, physical_id* physicalID);
	};
};

#endif // __STORE_MAP_H__
