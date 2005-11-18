#include "Map.h"

namespace Store
{
	Map::Map(DBStoreManager* store)
	{
		this->store = store;
	};

	Map::~Map()
	{
	}

	int Map::initializeMap(File* file)
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
		header->next_id = 0;
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

		return STORE_ERR_SUCCESS;
	};
}
