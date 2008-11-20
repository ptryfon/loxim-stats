#ifndef __STORE_MAP_H__
#define __STORE_MAP_H__

namespace Store
{
	class Map;
};

#include <Store/Store.h>
#include <Store/Struct.h>
#include <Store/File.h>
#include <Store/Buffer.h>
#include <Store/PagePointer.h>
#include <Log/Logs.h>

#define STORE_MAP_PERPAGE			((STORE_PAGESIZE - sizeof(map_page)) / sizeof(physical_id))
#define STORE_MAP_MAPPAGE(i)		(1 + ((i) / STORE_MAP_PERPAGE))
#define STORE_MAP_MAPOFFSET(i)		(sizeof(map_page) + sizeof(physical_id) * ((i) % STORE_MAP_PERPAGE))

using namespace std;
using namespace TManager;

namespace Store
{
	class Map
	{
	private:
		Buffer* buffer;
		LogManager* log;
		PagePointer* header;

		unsigned int getLastAssigned(TransactionID* tid);
		void setLastAssigned(TransactionID* tid, unsigned int);

	public:
		Map();
		~Map();

		physical_id* RIP;
		bool equal(physical_id* a, physical_id* b);

		void init(Buffer* buffer, LogManager* log);
		int initializeFile(File* file);
		int initializePage(unsigned int pageID, char* page);

		unsigned int createLogicalID(TransactionID* tid);
		int getPhysicalID(TransactionID* tid, unsigned int logicalID, physical_id** physicalID);
		int setPhysicalID(TransactionID* tid, unsigned int logicalID, physical_id* physicalID);
	};
};

#endif // __STORE_MAP_H__
