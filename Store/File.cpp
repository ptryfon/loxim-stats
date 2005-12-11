#include "File.h"
#include "Errors/Errors.h"

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

	int File::getStream(unsigned short fileID, fstream** file)
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

		fmap = new fstream();
		froots = new fstream();
		fdefault = new fstream();

		fmap->open("sbmap", ios::in | ios::binary);
		froots->open("sbroots", ios::in | ios::binary);
		fdefault->open("sbdefault", ios::in | ios::binary);

		if (!(fmap->is_open()) || !froots->is_open() || !fdefault->is_open())
		{
			if (fmap->is_open()) fmap->close();
			if (froots->is_open()) froots->close();
			if (fdefault->is_open()) fdefault->close();

			fmap->open("sbmap", ios::out | ios::binary | ios::trunc);
			froots->open("sbroots", ios::out | ios::binary | ios::trunc);
			fdefault->open("sbdefault", ios::out | ios::binary | ios::trunc);

			store->getMap()->initializeFile(this);
			store->getRoots()->initializeFile(this);

			if (fmap->is_open()) fmap->close();
			if (froots->is_open()) froots->close();
			if (fdefault->is_open()) fdefault->close();
		}

		fmap->open("sbmap", ios::in | ios::out | ios::binary);
		froots->open("sbroots", ios::in | ios::out | ios::binary);
		fdefault->open("sbdefault", ios::in | ios::out | ios::binary);

		started = 1;
		return 0;
	};

	int File::stop()
	{
		if (!started)
			return 0;

		if (fdefault && fdefault->is_open()) { fdefault->flush(); fdefault->close(); }
		if (fmap && fmap->is_open()) { fmap->flush(); fmap->close(); }
		if (froots && froots->is_open()) { froots->flush(); froots->close(); }

		if (fdefault) { delete fdefault; fdefault = 0; }
		if (fmap) { delete fmap; fmap = 0; }
		if (froots) { delete froots; froots = 0; }

		started = 0;
		return 0;
	};

	int File::read(unsigned short fileID, unsigned int offset, int length, char* buffer)
	{
		fstream* file = 0;
		int err = 0;

		if ((err = getStream(fileID, &file)) != 0)
			return err;

		file->seekg(offset, ios::beg);
		file->read(buffer, length);

		return 0;
	};

	int File::readPage(unsigned short fileID, unsigned int pageOffset, char* buffer)
	{
		return this->read(fileID, pageOffset * STORE_PAGESIZE, STORE_PAGESIZE, buffer);
	};

	int File::write(unsigned short fileID, unsigned int offset, int length, char* buffer)
	{
		fstream* file = 0;
		int err = 0;

		if ((err = getStream(fileID, &file)) != 0)
			return err;

		file->seekp(offset, ios::beg);
		file->write(buffer, length);

		return 0;
	};

	int File::writePage(unsigned short fileID, unsigned int pageOffset, char* buffer)
	{
		return this->write(fileID, pageOffset * STORE_PAGESIZE, STORE_PAGESIZE, buffer);
	};
}
