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

#include <map>

using namespace std;

namespace Store
{
	class Buffer
	{
	private:
		typedef struct buffer_page
		{
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
		int started;

	public:
		Buffer(DBStoreManager* store);
		~Buffer();
		int start();
		int stop();

		PagePointer* getPagePointer(unsigned short fileID, unsigned int pageID);
		int writePage(unsigned short fileID, unsigned int pageID, char *pagePointer);
	};
};

#endif // __STORE_BUFFER_H__
