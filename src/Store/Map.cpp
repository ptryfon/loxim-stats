#include <Store/Map.h>

namespace Store
{
	Map::Map()
	{
		RIP = (physical_id*) new char[sizeof(physical_id)];
		memset(RIP, 0xFF, sizeof(physical_id));
	};

	Map::~Map()
	{
		delete RIP;

		if (header)
			delete header;
	}

	bool Map::equal(physical_id* a, physical_id* b)
	{
		return memcmp(a, b, sizeof(physical_id)) == 0;
	}

	void Map::init(Buffer* buffer, LogManager* log)
	{
		this->buffer = buffer;
		this->log = log;
	}

	int Map::initializeFile(File* file)
	{
		char* buf = new char[STORE_PAGESIZE];
		map_header* header = (map_header*) buf;
		map_page* page = (map_page*) buf;

		memset(buf, 0, STORE_PAGESIZE);
		header->page_hdr.file_id = STORE_FILE_MAP;
		header->page_hdr.page_id = 0;
		header->page_hdr.page_type = STORE_PAGE_MAPHEADER;
		header->page_hdr.timestamp = 0;
		header->page_count = 16;
		header->last_assigned = 0;
		file->writePage(STORE_FILE_MAP, 0, buf);

		memset(buf, 0, STORE_PAGESIZE);
		page->page_hdr.file_id = STORE_FILE_MAP;
		page->page_hdr.page_type = STORE_PAGE_MAPPAGE;

		for (int i = 1; i < 16; i++)
		{
			page->page_hdr.page_id = i;
			file->writePage(STORE_FILE_MAP, i, buf);
		}

		delete buf;

		return 0;
	};

	int Map::initializePage(unsigned int pageID, char* page)
	{
		return 0;
	}

	unsigned int Map::getLastAssigned(TransactionID* tid)
	{
		unsigned int last;

//		if (!header)
			header = buffer->getPagePointer(tid, STORE_FILE_MAP, 0);

		header->aquire(tid);
		last = ((map_header*) header->getPage())->last_assigned;
		header->release(tid, 0);

		return last;
	};

	void Map::setLastAssigned(TransactionID* tid, unsigned int last)
	{
		if (!header)
			header = buffer->getPagePointer(tid, STORE_FILE_MAP, 0);

		header->aquire(tid);
		((page_header*) header->getPage())->timestamp = log->getLogicalTimerValue();
		((map_header*) header->getPage())->last_assigned = last;
		header->releaseSync(tid, 1);
	};

	unsigned int Map::createLogicalID(TransactionID* tid)
	{
		unsigned int last = getLastAssigned(tid);
		PagePointer* page = buffer->getPagePointer(tid, STORE_FILE_MAP, STORE_MAP_MAPPAGE(last + 1));

		page->aquire(tid);

		memset(page->getPage() + STORE_MAP_MAPOFFSET(last + 1), 0xFF, sizeof(physical_id));
		((page_header*) page->getPage())->timestamp = log->getLogicalTimerValue();

		page->releaseSync(tid, 1);

		setLastAssigned(tid, last + 1);
		delete page;

		return last + 1;
	};

	int Map::getPhysicalID(TransactionID* tid, unsigned int logicalID, physical_id** physicalID)
	{
		*physicalID = 0;

		if (!logicalID)
			return 1;

		unsigned int last = getLastAssigned(tid);

		if (last < logicalID)
			return 2;

		PagePointer* page = buffer->getPagePointer(tid, STORE_FILE_MAP, STORE_MAP_MAPPAGE(logicalID));

		page->aquire(tid);

		*physicalID = (physical_id*) new char[sizeof(physical_id)];
		memcpy(*physicalID, page->getPage() + STORE_MAP_MAPOFFSET(logicalID), sizeof(physical_id));

		page->release(tid, 0);
		delete page;

		return 0;
	};

	int Map::setPhysicalID(TransactionID* tid, unsigned int logicalID, physical_id* physicalID)
	{
		if (!logicalID || !physicalID)
			return 1;

		unsigned int last = getLastAssigned(tid);

		if (logicalID > 10000000) // md
			return 2;              // md

		if (last < logicalID) {
			int ll;                                   // md
			physical_id pp;                           // md
			memset(&pp, 0xFF, sizeof pp);             // md
			while(getLastAssigned(tid) < logicalID) {    // md
				ll = createLogicalID(tid);                // md
				setPhysicalID(tid, ll, &pp);                // md
			}                                         // md
		//	return 2;
		}

		PagePointer* page = buffer->getPagePointer(tid, STORE_FILE_MAP, STORE_MAP_MAPPAGE(logicalID));
		page->aquire(tid);

		memcpy(page->getPage() + STORE_MAP_MAPOFFSET(logicalID), physicalID, sizeof(physical_id));
		((page_header*) page->getPage())->timestamp = log->getLogicalTimerValue();

		page->releaseSync(tid, 1);
		delete page;

		return 0;
	};
}
