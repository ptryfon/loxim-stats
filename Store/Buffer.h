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

		DBStoreManager* store;
		File* file;
		int started;
		int buffer_count;
		buffer_page* buffer_table;

	public:
		Buffer(DBStoreManager* store);
		~Buffer();
		int start();
		int stop();

		PagePointer* getPagePointer(unsigned short fileID, unsigned int pageID);
	};
};

#endif // __STORE_BUFFER_H__
