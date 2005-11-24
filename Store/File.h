#ifndef __STORE_FILEMANAGER_H__
#define __STORE_FILEMANAGER_H__

namespace Store
{
	class File;
};

#include <fstream>
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

		fstream* fmap;
		fstream* froots;
		fstream* fdefault;

	private:
		int getStream(unsigned short fileID, fstream** file);

	public:
		File(DBStoreManager* store);
		~File();
		int start();
		int stop();
		int read(unsigned short fileID, unsigned int offset, int length, char* buffer);
		int readPage(unsigned short fileID, unsigned int pageOffset, char* buffer);
		int write(unsigned short fileID, unsigned int offset, int length, char* buffer);
		int writePage(unsigned short fileID, unsigned int pageOffset, char* buffer);
	};
};

#endif // __STORE_FILEMANAGER_H__
