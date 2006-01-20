#include "File.h"
#include "Errors/Errors.h"

#include <iostream>
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
	};

	File::~File()
	{
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

		fmap = open("/tmp/sbmap", O_RDWR);
		froots = open("/tmp/sbroots", O_RDWR);
		fdefault = open("/tmp/sbdefault", O_RDWR);

		if (fmap == -1 || froots == -1 || fdefault == -1)
		{
			if (fmap > 0) close(fmap);
			if (froots > 0) close(froots);
			if (fdefault > 0) close(fdefault);

			fmap = open("/tmp/sbmap", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
			froots = open("/tmp/sbroots", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
			fdefault = open("/tmp/sbdefault", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

			store->getMap()->initializeFile(this);
			store->getRoots()->initializeFile(this);
		}

		started = 1;
		return 0;
	};

	int File::stop()
	{
		if (!started)
			return 0;

		if (fmap > 0) close(fmap);
		if (froots > 0) close(froots);
		if (fdefault > 0) close(fdefault);

		started = 0;
		return 0;
	};

	int File::fread(unsigned short fileID, unsigned int offset, int length, char* buffer)
	{
		int file = 0;
		int err = 0;

		if ((err = getStream(fileID, &file)) != 0)
			return err;

		lseek(file, offset, SEEK_SET);
		read(file, buffer, length);

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

		lseek(file, offset, SEEK_SET);
		write(file, buffer, length);

		return 0;
	};

	int File::writePage(unsigned short fileID, unsigned int pageOffset, char* buffer)
	{
		return this->fwrite(fileID, pageOffset * STORE_PAGESIZE, STORE_PAGESIZE, buffer);
	};
}
