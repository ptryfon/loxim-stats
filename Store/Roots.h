#ifndef __STORE_ROOTS_H__
#define __STORE_ROOTS_H__

namespace Store
{
	class Roots;
};

#include <vector>
#include "Store.h"
#include "Struct.h"
#include "DBStoreManager.h"
#include "File.h"
#include "PagePointer.h"

#define STORE_ROOTS_PERPAGE			((STORE_PAGESIZE - sizeof(roots_page)) / sizeof(unsigned int))
#define STORE_ROOTS_ROOTPAGE(i)		(1 + (i / STORE_ROOTS_PERPAGE))
#define STORE_ROOTS_ROOTOFFSET(i)	(sizeof(roots_page) + sizeof(unsigned int) * (i % STORE_ROOTS_PERPAGE))

using namespace std;

namespace Store
{
	class Roots
	{
	private:
		DBStoreManager* store;
		PagePointer* header;

		unsigned int getLastAssigned();
		void setLastAssigned(unsigned int);

	public:
		Roots(DBStoreManager* storemgr);
		~Roots();

		int initializeFile(File* file);

		int addRoot(unsigned int logicalID);
		int removeRoot(unsigned int logicalID);
		vector<int>* getRoots();
	};
};

#endif // __STORE_ROOTS_H__
