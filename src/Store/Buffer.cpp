/**
 * $Id: Buffer.cpp,v 1.39 2008-07-24 21:55:27 dk209472 Exp $
 *
 */
#include <Store/Buffer.h>
#include <Log/Logs.h>
#include <SystemStats/AllStats.h>

using namespace std;
using namespace SystemStatsLib;

namespace Store
{
	Buffer::Buffer(DBStoreManager* store)
	{
		this->store = store;
		this->file = new File(store);
		this->started = 0;
		this->ec = &ErrorConsole::get_instance(EC_STORE);
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
		int retval;

		if (started)
			return 0;

		if ((retval = file->start()) != 0)
			return retval;

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

	int Buffer::readPage(TransactionID* tid, unsigned short fileID, unsigned int pageID, buffer_page*& n_page)
	{
		int retval;
		if (tid != NULL) {
			AllStats::getHandle()->addDiskPageReads(tid->getSessionId(), tid->getId(), 1);
		} else {
			AllStats::getHandle()->addDiskPageReads(-1, -1, 1);
		}
		n_page = new buffer_page;
		n_page->page = new char[STORE_PAGESIZE];

		if ((retval = file->readPage(fileID, pageID, n_page->page)) != 0) {
			delete n_page->page;
			delete n_page;
			return retval;
		}

		n_page->haspage = 1;
		n_page->dirty = 0;
		n_page->lock = 0;

		return 0;
	}

	PagePointer* Buffer::getPagePointer(TransactionID* tid, unsigned short fileID, unsigned int pageID)
	{
		unsigned int pnum;
		buffer_page* n_page;

		if (!started) {
		    return 0;
		}

		::pthread_mutex_lock(&dbwriter.mutex);
		buffer_addr_t buffer_addr = make_pair(fileID, pageID);
		buffer_hash_t::iterator it = buffer_hash.find(buffer_addr);

		if (it != buffer_hash.end() && (*it).second.haspage) {
			n_page = &((*it).second);
			::pthread_mutex_unlock(&dbwriter.mutex);
			return new PagePointer(fileID, pageID, n_page->page, this);
		}

		if ((pnum = file->hasPage(fileID, pageID)) > pageID) {
			if (readPage(tid, fileID, pageID, n_page) < 0) {
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

						case STORE_FILE_VIEWS:
							store->getViews()->initializePage(i, n_page->page);
							break;

						//TODO - opisacjako zmieniane miejsce
						case STORE_FILE_CLASSES:
							store->getClasses()->initializePage(i, n_page->page);
							break;
						case STORE_FILE_INTERFACES:
							store->getInterfaces()->initializePage(i, n_page->page);
							break;
						case STORE_FILE_SCHEMAS:
							store->getSchemas()->initializePage(i, n_page->page);
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

	int Buffer::acquirePage(TransactionID* tid, PagePointer* pp)
	{
		int retval;

		if (!started)
			return -1;

		::pthread_mutex_lock(&dbwriter.mutex);

		if (tid != NULL) {
			AllStats::getHandle()->addPageReads(tid->getSessionId(), tid->getId(), 1);
		} else {
			AllStats::getHandle()->addPageReads(-1, -1, 1);
		}
		buffer_addr_t buffer_addr = make_pair(pp->getFileID(), pp->getPageID());
		buffer_hash_t::iterator it = buffer_hash.find(buffer_addr);
		buffer_page* n_page;

		if (it != buffer_hash.end() && (*it).second.haspage) {
			n_page = &((*it).second);
			n_page->lock++;

			::pthread_mutex_unlock(&dbwriter.mutex);
			return 0;
		} else if (it != buffer_hash.end()) {
			if ((retval = readPage(tid, pp->getFileID(), pp->getPageID(), n_page)) != 0) {
				::pthread_mutex_unlock(&dbwriter.mutex);
				return retval;
			}

			n_page->lock++;
			(*it).second = *n_page;
			pp->setPage(n_page->page);

			::pthread_mutex_unlock(&dbwriter.mutex);
			return 0;
		} else {
			::pthread_mutex_unlock(&dbwriter.mutex);
			return -1;
		}
	};

	int Buffer::acquirePageRead(PagePointer* /*pp*/)
	{
		return -1;
/*		int retval;

		if (!started)
			return -1;

		::pthread_mutex_lock(&dbwriter.mutex);
		buffer_addr_t buffer_addr = make_pair(pp->getFileID(), pp->getPageID());
		buffer_hash_t::iterator it = buffer_hash.find(buffer_addr);
		buffer_page* n_page;

		if (it != buffer_hash.end() && (*it).second.haspage) {
		} else if (it != buffer_hash.end()) {
		} else {
			::pthread_mutex_unlock(&dbwriter.mutex);
			return -1;
		}*/
	};

	int Buffer::releasePage(TransactionID* tid, PagePointer* pp)
	{
		if (!started)
			return -1;

		::pthread_mutex_lock(&dbwriter.mutex);

		if (tid != NULL) {
					AllStats::getHandle()->addPageWrites(tid->getSessionId(), tid->getId(), 1);
				} else {
					AllStats::getHandle()->addPageWrites(-1, -1, 1);
				}

		buffer_addr_t buffer_addr = make_pair(pp->getFileID(), pp->getPageID());
		buffer_hash_t::iterator it = buffer_hash.find(buffer_addr);
		buffer_page* n_page;

		if (it != buffer_hash.end() && (*it).second.haspage) {
			n_page = &((*it).second);

			n_page->lock--;

			if (n_page->dirty != 1) {
				if (dbwriter.dirty_pages < max_dirty)
					::pthread_cond_signal(&dbwriter.cond);

				n_page->dirty = 1;
				if (tid != NULL) {
							AllStats::getHandle()->addDiskPageWrites(tid->getSessionId(), tid->getId(), 1);
						} else {
							AllStats::getHandle()->addDiskPageWrites(-1, -1, 1);
						}
				dbwriter.dirty_pages++;
			}

			::pthread_mutex_unlock(&dbwriter.mutex);
			return 0;
		} else {
			::pthread_mutex_unlock(&dbwriter.mutex);
			return -1;
		}
	};

	int Buffer::releasePageSync(TransactionID* tid, PagePointer* pp)
	{
		if (!started)
			return -1;

		::pthread_mutex_lock(&dbwriter.mutex);
		buffer_addr_t buffer_addr = make_pair(pp->getFileID(), pp->getPageID());
		buffer_hash_t::iterator it = buffer_hash.find(buffer_addr);
		buffer_page* n_page;

		if (it != buffer_hash.end() && (*it).second.haspage) {
			n_page = &((*it).second);
			if (tid != NULL) {
				AllStats::getHandle()->addPageWrites(tid->getSessionId(), tid->getId(), 1);
				AllStats::getHandle()->addDiskPageWrites(tid->getSessionId(), tid->getId(), 1);
			} else {
				AllStats::getHandle()->addPageWrites(-1, -1, 1);
				AllStats::getHandle()->addDiskPageWrites(-1, -1, 1);
			}
			file->writePage((*it).first.first, (*it).first.second, n_page->page);
			n_page->dirty = 0;
			dbwriter.dirty_pages--;

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
				debug_printf(*ec, "Store::Buffer: dbwriter started with dirty(%d)\n", dbwriter.dirty_pages);
				dbWriterWrite();
				debug_printf(*ec, "Store::Buffer: dbwriter finished with dirty(%d)\n", dbwriter.dirty_pages);
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

		store->checkpoint(cid);

		it = buffer_hash.begin();
		while (it != buffer_hash.end()) {
			n_page = &((*it).second);

			if (n_page->haspage && (n_page->lock == 0) && n_page->dirty) {
				debug_printf(*ec, "Store::Buffer: dbwriter writing(file:%d, page:%d)\n", (*it).first.first, (*it).first.second);
				file->writePage((*it).first.first, (*it).first.second, n_page->page);
				n_page->dirty = 0;
				dbwriter.dirty_pages--;
			}

			it++;
		}

		store->endCheckpoint(cid);

		return 0;
	};
};
