#include "Roots.h"

namespace Store
{
	Roots::Roots(DBStoreManager* store)
	{
		this->store = store;
	};

	Roots::~Roots()
	{
		if (header)
			delete header;
	};

	int Roots::initializeFile(File* file)
	{
		char* buf = new char[STORE_PAGESIZE];
		roots_header* header = (roots_header*) buf;
		roots_page* page = (roots_page*) buf;

		memset(buf, 0, STORE_PAGESIZE);
		header->page_hdr.file_id = STORE_FILE_ROOTS;
		header->page_hdr.page_id = 0;
		header->page_hdr.page_type = STORE_PAGE_ROOTSHEADER;
		header->page_hdr.timestamp = 0;
		header->page_count = 16;
		header->last_assigned = 0;
		file->writePage(STORE_FILE_ROOTS, 0, buf);

		memset(buf, 0, STORE_PAGESIZE);
		page->page_hdr.file_id = STORE_FILE_ROOTS;
		page->page_hdr.page_type = STORE_PAGE_ROOTSPAGE;

		for (int i = 1; i < 16; i++)
		{
			page->page_hdr.page_id = i;
			file->writePage(STORE_FILE_ROOTS, i, buf);
		}

		delete buf;

		return 0;
	};

	unsigned int Roots::getLastAssigned()
	{
		unsigned int last;

		if (!header)
			store->getBuffer()->getPagePointer(STORE_FILE_ROOTS, 0);

		header->aquire();
		last = ((roots_header*) header->getPage())->last_assigned;
		header->release();

		return last;
	};

	void Roots::setLastAssigned(unsigned int last)
	{
		if (!header)
			store->getBuffer()->getPagePointer(STORE_FILE_ROOTS, 0);

		header->aquire();
		((roots_header*) header->getPage())->last_assigned = last;
		header->release();
	};

	int Roots::addRoot(unsigned int logicalID)
	{
		if (!logicalID)
			return 1;

		unsigned int last = getLastAssigned();
		PagePointer* page = store->getBuffer()->getPagePointer(STORE_FILE_ROOTS, STORE_ROOTS_ROOTPAGE(last + 1));

		page->aquire();
		*((unsigned int*) (page->getPage() + STORE_ROOTS_ROOTOFFSET(last + 1))) = logicalID;
		page->release();

		setLastAssigned(last + 1);
		delete page;

		return 0;
	};

	int Roots::removeRoot(unsigned int logicalID)
	{
		if (!logicalID)
			return 1;

		unsigned int last = getLastAssigned();
		PagePointer* page = 0;
		int found = 0;

		for (unsigned int i = 0; i < last && !found; i++)
		{
			unsigned int pageid = STORE_ROOTS_ROOTPAGE(i);

			if (!page)
				page = store->getBuffer()->getPagePointer(STORE_FILE_ROOTS, pageid);
			else if (page->getPageID() != pageid)
			{
				delete page;
				page = store->getBuffer()->getPagePointer(STORE_FILE_ROOTS, pageid);
			}

			page->aquire();

			unsigned int root = *((unsigned int*) (page->getPage() + STORE_ROOTS_ROOTOFFSET(i)));
			if (root && root == logicalID)
			{
				found = 1;
				*((unsigned int*) (page->getPage() + STORE_ROOTS_ROOTOFFSET(i))) = 0;
			}

			page->release();
		}

		if (page)
			delete page;

		return 0;
	};

	vector<int>* Roots::getRoots()
	{
		vector<int>* roots = new vector<int>();
		unsigned int last = getLastAssigned();
		PagePointer* page = 0;

		for (unsigned int i = 0; i < last; i++)
		{
			int pageid = STORE_ROOTS_ROOTPAGE(i);

			if (!page)
				page = store->getBuffer()->getPagePointer(STORE_FILE_ROOTS, pageid);
			else if (page->getPageID() != pageid)
			{
				delete page;
				page = store->getBuffer()->getPagePointer(STORE_FILE_ROOTS, pageid);
			}

			page->aquire();

			unsigned int root = *((unsigned int*) (page->getPage() + STORE_ROOTS_ROOTOFFSET(i)));
			if (root)
				roots->push_back(root);

			page->release();
		}

		if (page)
			delete page;

		return roots;
	};
}
