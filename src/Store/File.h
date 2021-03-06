#ifndef __STORE_FILEMANAGER_H__
#define __STORE_FILEMANAGER_H__

namespace Store
{
	class File;
};

#include <Store/Store.h>
#include <Store/Struct.h>
#include <Store/DBStoreManager.h>
#include <Errors/ErrorConsole.h>

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
		int fviews;
		int fclasses;
		int finterfaces;
		int fschemas;;


		int getStream(unsigned short fileID, int* file);

		Errors::ErrorConsole* ec;

	public:
		File(DBStoreManager* store);
		~File();
		int start();
		int stop();
		int fread(unsigned short fileID, unsigned int offset, int length, char* buffer);
		int readPage(unsigned short fileID, unsigned int pageOffset, char* buffer);
		int fwrite(unsigned short fileID, unsigned int offset, int length, char* buffer);
		int writePage(unsigned short fileID, unsigned int pageOffset, char* buffer);
		unsigned int hasPage(unsigned short fileID, unsigned int pageOffset);
	};
};

#endif // __STORE_FILEMANAGER_H__
