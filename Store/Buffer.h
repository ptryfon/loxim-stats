#ifndef __STORE_BUFFER_H__
#define __STORE_BUFFER_H__

namespace Store
{
	class Buffer;
};

#include "DBStoreManager.h"
#include "File.h"
#include "PagePointer.h"
#include "Struct.h"
#include "Errors/ErrorConsole.h"

#include <pthread.h>

#include <map>

using namespace std;

namespace Store
{
	class Buffer
	{
	private:
		typedef struct buffer_page
		{
			unsigned short fileID;
			unsigned int pageID;
			unsigned char haspage;
			unsigned char dirty;
			unsigned char lock;
			unsigned int id;
			char* page;
		} buffer_page;

		typedef pair<unsigned short, unsigned short> buffer_addr_t;
		typedef map<buffer_addr_t, buffer_page> buffer_hash_t;
		buffer_hash_t buffer_hash;

		int max_pages;
		int max_dirty;

		pthread_t tid_dbwriter;
		struct {
			pthread_mutex_t mutex;
			pthread_cond_t cond;
			int pages;
			int dirty_pages;
		} dbwriter;

		DBStoreManager* store;
		File* file;
		ErrorConsole *ec;

		int started;

	public:
		Buffer(DBStoreManager* store);
		~Buffer();
		int start();
		int stop();

		PagePointer* getPagePointer(unsigned short fileID, unsigned int pageID);
		int readPage(unsigned short fileID, unsigned int pageID, buffer_page*& n_page);
		int acquirePage(PagePointer* pp);
		int releasePage(PagePointer* pp);
		static void* dbWriterThread(void* arg);
		void* dbWriterMain(void);
		int dbWriterWrite(void);
	};
};

#endif // __STORE_BUFFER_H__
