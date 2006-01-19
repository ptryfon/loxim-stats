#ifndef __STORE_FILEMANAGER_H__
#define __STORE_FILEMANAGER_H__

namespace Store
{
	class File;
};

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "Store.h"
#include "Struct.h"
#include "DBStoreManager.h"

using namespace std;

namespace Store
{
	class File
	{
	private:
		DBStoreManager* store;
		int started;

		int fmap;
		int froots;
		int fdefault;

	private:
		int getStream(unsigned short fileID, int* file);

	public:
		File(DBStoreManager* store);
		~File();
		int start();
		int stop();
		int fread(unsigned short fileID, unsigned int offset, int length, char* buffer);
		int readPage(unsigned short fileID, unsigned int pageOffset, char* buffer);
		int fwrite(unsigned short fileID, unsigned int offset, int length, char* buffer);
		int writePage(unsigned short fileID, unsigned int pageOffset, char* buffer);
	};
};

#endif // __STORE_FILEMANAGER_H__
