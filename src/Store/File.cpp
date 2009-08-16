#include <Store/File.h>
#include <Errors/Errors.h>

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <SystemStats/AllStats.h>
#include <time.h>
#include <sys/time.h>

namespace Store
{
	File::File(DBStoreManager* store)
	{
		this->store = store;
		this->started = 0;

		this->fmap = 0;
		this->froots = 0;
		this->fdefault = 0;
		this->fviews = 0;
		this->fclasses = 0;
		this->finterfaces = 0;
		this->fschemas = 0;

		this->ec = &ErrorConsole::get_instance(EC_STORE_FILE);
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
		else if (fileID == STORE_FILE_VIEWS)
			*file = fviews;
		else if (fileID == STORE_FILE_CLASSES)
			*file = fclasses;
		else if (fileID == STORE_FILE_INTERFACES)
			*file = finterfaces;
		else if (fileID == STORE_FILE_SCHEMAS)
			*file = fschemas;
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

		string smap, sroots, sdefault, sviews, sclasses, sinterfaces, sschemas;

		if (store->getConfig()->getString("store_file_default", sdefault) != 0)
			sdefault = "/tmp/sbdefault";
		if (store->getConfig()->getString("store_file_roots", sroots) != 0)
			sroots = "/tmp/sbroots";
		if (store->getConfig()->getString("store_file_map", smap) != 0)
			smap = "/tmp/sbmap";
		if (store->getConfig()->getString("store_file_views", sviews) != 0)
			sviews = "/tmp/sbviews";
		if (store->getConfig()->getString("store_file_fclasses", sclasses) != 0)
			sclasses = "/tmp/sbclasses";
		if (store->getConfig()->getString("store_file_interfaces", sinterfaces) != 0)
			sinterfaces = "/tmp/sbinterfaces";
		if (store->getConfig()->getString("store_file_schemas", sschemas) != 0)
			sschemas = "/tmp/sbschemas";

		fmap = ::open(smap.c_str(), O_RDWR);
		debug_printf(*ec, "fmap = %d, errno = %d, errmsg = %s",
			fmap, errno, strerror(errno));
		froots = ::open(sroots.c_str(), O_RDWR);
		debug_printf(*ec, "froots = %d, errno = %d, errmsg = %s",
			froots, errno, strerror(errno));
		fdefault = ::open(sdefault.c_str(), O_RDWR);
		debug_printf(*ec, "fdefault = %d, errno = %d, errmsg = %s",
			fdefault, errno, strerror(errno));
		fviews = ::open(sviews.c_str(), O_RDWR);
		debug_printf(*ec, "fviews = %d, errno = %d, errmsg = %s",
			fviews, errno, strerror(errno));
		fclasses = ::open(sclasses.c_str(), O_RDWR);
		debug_printf(*ec, "fclasses = %d, errno = %d, errmsg = %s",
			fclasses, errno, strerror(errno));
		finterfaces = ::open(sinterfaces.c_str(), O_RDWR);
		debug_printf(*ec, "finterfaces = %d, errno = %d, errmsg = %s",
			finterfaces, errno, strerror(errno));
		fschemas = ::open(sschemas.c_str(), O_RDWR);
		debug_printf(*ec, "fschemas = %d, errno = %d, errmsg = %s",
			fschemas, errno, strerror(errno));


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

		if (fviews == -1)
		{
			if ((fviews = ::open(sviews.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) == -1)
				return EBadFile;

			store->getViews()->initializeFile(this);
		}

		if (fclasses == -1)
		{
			if ((fclasses = ::open(sclasses.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) == -1)
				return EBadFile;

			store->getClasses()->initializeFile(this);
		}

		if (finterfaces == -1)
		{
			if ((finterfaces = ::open(sinterfaces.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) == -1)
				return EBadFile;

			store->getInterfaces()->initializeFile(this);
		}
		
		if (fschemas == -1)
		{
			if ((fschemas = ::open(sschemas.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) == -1)
				return EBadFile;

			store->getSchemas()->initializeFile(this);
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
		if (fviews > 0) ::close(fviews);
		if (fclasses > 0) ::close(fclasses);
		if (finterfaces > 0) ::close(finterfaces);
		if (fschemas > 0) ::close(fschemas);


		started = 0;
		return 0;
	};

	int File::fread(unsigned short fileID, unsigned int offset, int length, char* buffer)
	{
		int file = 0;
		int err = 0;

		if ((err = getStream(fileID, &file)) != 0)
			return err;
		timeval begin, end;

		gettimeofday(&begin,NULL);
		::lseek(file, offset, SEEK_SET);
		::read(file, buffer, length);
		gettimeofday(&end,NULL);

		double milisec = (end.tv_sec - begin.tv_sec) * 1000 + (end.tv_usec - begin.tv_usec) / 1000;
		AllStats::getHandle()->getStoreStats()->addReadTime(length, milisec);

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

	unsigned int File::hasPage(unsigned short fileID, unsigned int /*pageOffset*/)
	{
		int file, err;
		if ((err = getStream(fileID, &file)) != 0)
			return err;

		return (lseek(file, 0, SEEK_END) / STORE_PAGESIZE);
	};
}
