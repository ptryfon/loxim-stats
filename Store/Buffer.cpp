/**
 * $Id: Buffer.cpp,v 1.26 2006-01-26 20:01:10 mk189406 Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */
#include "Buffer.h"
#include "Log/Logs.h"

using namespace std;

namespace Store
{
	Buffer::Buffer(DBStoreManager* store)
	{
		this->store = store;
		this->file = new File(store);
		this->started = 0;
		this->ec = new ErrorConsole("Store");
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

		if (ec)
			delete ec;
	};

	int Buffer::start()
	{
		if (started)
			return 0;

		file->start();

		::pthread_mutex_init(&dbwriter.mutex, NULL);
		::pthread_cond_init(&dbwriter.cond, NULL);

		dbwriter.pages = 0;
		dbwriter.dirty_pages = 0;

		max_pages = 128;
		max_dirty = 16;
		if (store->getConfig() != NULL) {
			store->getConfig()->getInt("store_buffer_maxpages", max_pages);
			store->getConfig()->getInt("store_buffer_maxdirty", max_dirty);
		}

		::pthread_create(&tid_dbwriter, NULL, &dbWriterThread, this);

		started = 1;
		return 0;
	};

	int Buffer::stop()
	{
		if (!started)
			return 0;

		::pthread_mutex_lock(&dbwriter.mutex);
		dbWriterWrite();
		file->stop();
		started = 0;
		::pthread_cond_signal(&dbwriter.cond);
		::pthread_mutex_unlock(&dbwriter.mutex);
		::pthread_join(tid_dbwriter, NULL);
		return 0;
	};

	int Buffer::readPage(unsigned short fileID, unsigned int pageID, buffer_page*& n_page)
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
		n_page->lock = 0;

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
			n_page = &((*it).second);
			::pthread_mutex_unlock(&dbwriter.mutex);
			return new PagePointer(fileID, pageID, n_page->page, this);
		}

		if ((pnum = file->hasPage(fileID, pageID)) > pageID) {
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
							store->getPageManager()->initializePage(i, n_page->page); 
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
				
					n_page->haspage = 1;
					n_page->dirty = 1;
					n_page->lock = 0;
					dbwriter.dirty_pages++;
					buffer_hash.insert(make_pair (make_pair (fileID, i), *n_page));
				}
			}
			if (dbwriter.dirty_pages < max_dirty)
				::pthread_cond_signal(&dbwriter.cond);
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
		buffer_page* n_page;

		if (it != buffer_hash.end() && (*it).second.haspage) {
			n_page = &((*it).second);

			n_page->lock = 1;

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
		buffer_page* n_page;

		if (it != buffer_hash.end() && (*it).second.haspage) {
			n_page = &((*it).second);

			if (dbwriter.dirty_pages < max_dirty)
				::pthread_cond_signal(&dbwriter.cond);

			n_page->lock = 0;

			if (n_page->dirty != 1) {
				n_page->dirty = 1;
				dbwriter.dirty_pages++;
			}

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

			while (dbwriter.dirty_pages < max_dirty) {
				::pthread_cond_wait(&dbwriter.cond, &dbwriter.mutex);
				if (started == 0)
					break;
			}

			if (started == 1) {
				ec->printf("Store::Buffer: dbwriter start with dirty(%d)\n", dbwriter.dirty_pages);
				dbWriterWrite();
				ec->printf("Store::Buffer: dbwriter finish with dirty(%d)\n", dbwriter.dirty_pages);
			} else {
				::pthread_mutex_unlock(&dbwriter.mutex);
				break;
			}

			::pthread_mutex_unlock(&dbwriter.mutex);
		}

		return NULL;
	}

	int Buffer::dbWriterWrite(void)
	{
		buffer_hash_t::iterator it;
		buffer_page* n_page;
		unsigned int cid;

		store->getLogManager()->checkpoint(store->getTManager()->getTransactionsIds(), cid);

		it = buffer_hash.begin();
		while (it != buffer_hash.end()) {
			n_page = &((*it).second);

			if (n_page->haspage && (!n_page->lock) && n_page->dirty) {
				ec->printf("Store::Buffer: dbwriter writing(file:%d, page:%d)\n", (*it).first.first, (*it).first.second);
				file->writePage((*it).first.first, (*it).first.second, n_page->page);
				n_page->dirty = 0;
				dbwriter.dirty_pages--;
			}

			it++;
		}

		store->getLogManager()->endCheckpoint(cid);

		return 0;
	};
};
