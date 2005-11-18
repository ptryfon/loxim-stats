#ifndef __STORE_MAPMANAGER_H__
#define __STORE_MAPMANAGER_H__

namespace Store
{
	class Map;
};

#include "Store.h"
#include "Struct.h"
#include "ErrorCodes.h"
#include "DBStoreManager.h"
#include "File.h"

using namespace std;

namespace Store
{
	class Map
	{
	private:
		DBStoreManager* store;

	public:
		Map(DBStoreManager* storemgr);
		~Map();

		int initializeMap(File* file);
	};
};

#endif // __STORE_MAPMANAGER_H__
