/**
 * $Id: Buffer.cpp,v 1.11 2006-01-23 08:48:54 mk189406 Exp $
 *
 */
#include "Buffer.h"

#include <iostream>

using namespace std;

namespace Store
{
	Buffer::Buffer(DBStoreManager* store)
	{
		this->store = store;
		this->file = new File(store);
		this->started = 0;
	};

	Buffer::~Buffer()
	{
		if (started)
			stop();

		if (file)
		{
			delete file;
			file = 0;
		}
	};

	int Buffer::start()
	{
		if (started)
			return 0;

		file->start();

		::pthread_mutex_init(&dbwriter.mutex, NULL);
		::pthread_cond_init(&dbwriter.cond, NULL);
		dbwriter.dirty_pages = 0;
		dbwriter.max_dirty = 16;
		::pthread_create(&tid_dbwriter, NULL, &dbWriterThread, this);

		started = 1;
		return 0;
	};

	int Buffer::stop()
	{
		if (!started)
			return 0;

		::pthread_mutex_lock(&dbwriter.mutex);
		file->stop();
		started = 0;
		::pthread_mutex_unlock(&dbwriter.mutex);
		return 0;
	};

	int Buffer::readPage(unsigned short fileID, unsigned int pageID, buffer_page* n_page)
	{
		n_page = new buffer_page;
		n_page->page = new char[STORE_PAGESIZE];

		if (file->readPage(fileID, pageID, n_page->page) != 0) {
			delete n_page->page;
			delete n_page;
			return -1;
		}
		n_page->haspage = 1;
		n_page->dirty = 0;

		return 0;
	}

	PagePointer* Buffer::getPagePointer(unsigned short fileID, unsigned int pageID)
	{
		unsigned int pnum;
		buffer_page* n_page;

		if (!started)
			return 0;
		
		::pthread_mutex_lock(&dbwriter.mutex);
		buffer_addr_t buffer_addr = make_pair(fileID, pageID);

		buffer_hash_t::iterator it = buffer_hash.find(buffer_addr);
		if (it != buffer_hash.end() && (*it).second.haspage) {
			::pthread_mutex_unlock(&dbwriter.mutex);
			return new PagePointer(fileID, pageID, (*it).second.page, this);
		}

		if ((pnum = file->hasPage(fileID, pageID)) >= pageID) {
			if (readPage(fileID, pageID, n_page) < 0) {
				::pthread_mutex_unlock(&dbwriter.mutex);
				return NULL;
			}

			buffer_hash.insert(make_pair (make_pair (fileID, pageID), *n_page));
		} else {
			for (unsigned int i = pnum; i <= pageID; i++) {
				it = buffer_hash.find(make_pair(fileID, i));
				if (it == buffer_hash.end() || !(*it).second.haspage) {
				
					n_page = new buffer_page;
					n_page->page = new char[STORE_PAGESIZE];
					switch (fileID) {
						case STORE_FILE_DEFAULT: 
							PageManager::initializePage(i, n_page->page); 
							break;

						case STORE_FILE_MAP: 
							store->getMap()->initializePage(i, n_page->page); 
							break;

						case STORE_FILE_ROOTS: 
							store->getRoots()->initializePage(i, n_page->page); 
							break;

						default:
							break;
					}
				
					if (dbwriter.dirty_pages < dbwriter.max_dirty)
						::pthread_cond_signal(&dbwriter.cond);

					n_page->haspage = 1;
					n_page->dirty = 1;
					dbwriter.dirty_pages++;
					buffer_hash.insert(make_pair (make_pair (fileID, i), *n_page));
				}
			}
		}

		::pthread_mutex_unlock(&dbwriter.mutex);
		return new PagePointer(fileID, pageID, n_page->page, this);
	};

	int Buffer::aquirePage(unsigned short fileID, unsigned int pageID)
	{
		if (!started)
			return -1;

		::pthread_mutex_lock(&dbwriter.mutex);
		buffer_addr_t buffer_addr = make_pair(fileID, pageID);
		buffer_hash_t::iterator it = buffer_hash.find(buffer_addr);
		buffer_page n_page;

		if (it != buffer_hash.end() && (*it).second.haspage) {
			n_page = (*it).second;

			n_page.lock = 1;

			::pthread_mutex_unlock(&dbwriter.mutex);
			return 0;
		} else {
			::pthread_mutex_unlock(&dbwriter.mutex);
			return -1;
		}
	};

	int Buffer::releasePage(unsigned short fileID, unsigned int pageID)
	{
		if (!started)
			return -1;

		::pthread_mutex_lock(&dbwriter.mutex);
		buffer_addr_t buffer_addr = make_pair(fileID, pageID);
		buffer_hash_t::iterator it = buffer_hash.find(buffer_addr);
		buffer_page n_page;

		if (it != buffer_hash.end() && (*it).second.haspage) {
			n_page = (*it).second;

			if (dbwriter.dirty_pages < dbwriter.max_dirty)
				::pthread_cond_signal(&dbwriter.cond);

			n_page.lock = 0;
			n_page.dirty = 1;
			dbwriter.dirty_pages++;

			::pthread_mutex_unlock(&dbwriter.mutex);
			return 0;
		} else {
			::pthread_mutex_unlock(&dbwriter.mutex);
			return -1;
		}
	};

	void* Buffer::dbWriterThread(void* arg)
	{
		Buffer* b = (Buffer*) arg;
		return b->dbWriterMain();
	};

	void* Buffer::dbWriterMain(void) 
	{
		for ( ; ; ) {
			::pthread_mutex_lock(&dbwriter.mutex);

			while (dbwriter.dirty_pages < dbwriter.max_dirty)
				::pthread_cond_wait(&dbwriter.cond, &dbwriter.mutex);
			cout << "Store::Buffer : starting dbWriter dirty(" << dbwriter.dirty_pages <<
				")" << endl;
			if (started)
				dbWriterWrite();
			else
				break;

			::pthread_mutex_unlock(&dbwriter.mutex);
		}

		return NULL;
	}

	int Buffer::dbWriterWrite(void)
	{
		buffer_hash_t::iterator it;
		buffer_page n_page;

		it = buffer_hash.begin();
		while (it != buffer_hash.end()) {
			n_page = (*it).second;	

			if (n_page.haspage && n_page.lock == 0 && n_page.dirty == 1) {
				cout << "Store::Buffer : writing(" << (*it).first.first <<
					", " << (*it).first.second << ")" << endl;
				file->writePage((*it).first.first, (*it).first.second, n_page.page);
				n_page.dirty = 0;
				n_page.haspage = 0;
				delete n_page.page;
				dbwriter.dirty_pages--;
			}

			it++;
		}

		return 0;
	};
};
