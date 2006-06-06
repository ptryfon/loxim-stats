#include "File.h"
#include "Errors/Errors.h"

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

namespace Store
{
	File::File(DBStoreManager* store)
	{
		this->store = store;
		this->started = 0;

		this->fmap = 0;
		this->froots = 0;
		this->fdefault = 0;

		this->ec = new ErrorConsole("Store: File");
	};

	File::~File()
	{
		delete this->ec;
	}

	int File::getStream(unsigned short fileID, int* file)
	{
		if (fileID == STORE_FILE_DEFAULT)
			*file = fdefault;
		else if (fileID == STORE_FILE_MAP)
			*file = fmap;
		else if (fileID == STORE_FILE_ROOTS)
			*file = froots;
		else
		{
			*file = 0;
			return 1;
		}

		return 0;
	};

	int File::start()
	{
		if (started)
			return 0;

		string smap, sroots, sdefault;

		if (store->getConfig()->getString("store_file_default", sdefault) != 0)
			smap = "/tmp/sbmap";
		if (store->getConfig()->getString("store_file_roots", sroots) != 0)
			sroots = "/tmp/sbroots";
		if (store->getConfig()->getString("store_file_map", smap) != 0)
			sdefault = "/tmp/sbdefault";

		fmap = ::open(smap.c_str(), O_RDWR);
		ec->printf("fmap = %d, errno = %d, errmsg = %s",
			fmap, errno, strerror(errno));
		froots = ::open(sroots.c_str(), O_RDWR);
		ec->printf("froots = %d, errno = %d, errmsg = %s",
			froots, errno, strerror(errno));
		fdefault = ::open(sdefault.c_str(), O_RDWR);
		ec->printf("fdefault = %d, errno = %d, errmsg = %s",
			fdefault, errno, strerror(errno));


		if (fmap == -1)
		{
			if ((fmap = ::open(smap.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) == -1)
				return EBadFile;

			store->getMap()->initializeFile(this);
		}

		if (froots == -1)
		{
			if ((froots = ::open(sroots.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) == -1)
				return EBadFile;

			store->getRoots()->initializeFile(this);
		}


		if (fdefault == -1)
		{
			if ((fdefault = ::open(sdefault.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) == -1)
				return EBadFile;

			store->getPageManager()->initializeFile(this);
		}

		started = 1;
		return 0;
	};

	int File::stop()
	{
		if (!started)
			return 0;

		if (fmap > 0) ::close(fmap);
		if (froots > 0) ::close(froots);
		if (fdefault > 0) ::close(fdefault);

		started = 0;
		return 0;
	};

	int File::fread(unsigned short fileID, unsigned int offset, int length, char* buffer)
	{
		int file = 0;
		int err = 0;

		if ((err = getStream(fileID, &file)) != 0)
			return err;

		::lseek(file, offset, SEEK_SET);
		::read(file, buffer, length);

		return 0;
	};

	int File::readPage(unsigned short fileID, unsigned int pageOffset, char* buffer)
	{
		return this->fread(fileID, pageOffset * STORE_PAGESIZE, STORE_PAGESIZE, buffer);
	};

	int File::fwrite(unsigned short fileID, unsigned int offset, int length, char* buffer)
	{
		int file = 0;
		int err = 0;

		if ((err = getStream(fileID, &file)) != 0)
			return err;

		::lseek(file, offset, SEEK_SET);
		::write(file, buffer, length);

		return 0;
	};

	int File::writePage(unsigned short fileID, unsigned int pageOffset, char* buffer)
	{
		return this->fwrite(fileID, pageOffset * STORE_PAGESIZE, STORE_PAGESIZE, buffer);
	};

	unsigned int File::hasPage(unsigned short fileID, unsigned int pageOffset)
	{
		int file, err;
		if ((err = getStream(fileID, &file)) != 0)
			return err;

		return (lseek(file, 0, SEEK_END) / STORE_PAGESIZE);
	};
}
