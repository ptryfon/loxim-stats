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
			char* page;
		} buffer_page;

		typedef pair<unsigned short, unsigned short> buffer_addr_t;
		typedef map<buffer_addr_t, buffer_page> buffer_hash_t;
		buffer_hash_t buffer_hash;

		DBStoreManager* store;
		File* file;

		pthread_t tid_dbwriter;
		struct {
			pthread_mutex_t mutex;
			pthread_cond_t cond;
			int dirty_pages;
			int max_dirty;
		} dbwriter;

		int started;

	public:
		Buffer(DBStoreManager* store);
		~Buffer();
		int start();
		int stop();

		PagePointer* getPagePointer(unsigned short fileID, unsigned int pageID);
		int readPage(unsigned short fileID, unsigned int pageID, buffer_page* n_page);
		int aquirePage(unsigned short fileID, unsigned int pageID);
		int releasePage(unsigned short fileID, unsigned int pageID);
		static void* dbWriterThread(void* arg);
		void* dbWriterMain(void);
		int dbWriterWrite(void);
	};
};

#endif // __STORE_BUFFER_H__
