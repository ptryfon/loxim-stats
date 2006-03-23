#ifndef __STORE_NAMEDROOTS_H__
#define __STORE_NAMEDROOTS_H__

#define STORE_ROOTS_INITIALPAGECOUNT	16
#define STORE_ROOTS_NAMEMAXLEN			100
#define STORE_ROOTS_NULLVALUE			0

namespace Store
{
	class NamedRoots;
};

#include <stdlib.h>
#include <vector>
#include "Store.h"
#include "Struct.h"
#include "DBStoreManager.h"
#include "File.h"
#include "PagePointer.h"
#include "Errors/ErrorConsole.h"

using namespace std;

namespace Store
{
	class NamedRoots
	{
	private:
		typedef struct namedroots_header
		{
			page_header page_hdr;
		} namedroots_header;

		typedef struct namedroots_page
		{
			page_header page_hdr;
			int free;
		} namedroots_page;

		typedef struct namedroots_entry
		{
			int size;
			int l_id;
			int cur_tran;
			int add_t;
			int del_t;
			char name[];
		} namedroots_entry;

		DBStoreManager* store;
		ErrorConsole* ec;

		int modifyTransaction(int transactionID, char mode);

	public:
		NamedRoots(DBStoreManager* storemgr);
		~NamedRoots();

		int initializeFile(File* file);
		int initializePage(unsigned int pageID, char* page);

		int commitTransaction(int transactionID);
		int abortTransaction(int transactionID);
		int abortAllTransactions();

		int addRoot(int logicalID, const char* name, int transactionID, int transactionTimeStamp);
		int removeRoot(int logicalID, int transactionID, int transactionTimeStamp);
		vector<int>* getRoots(int transactionID, int transactionTimeStamp);
		vector<int>* getRoots(const char* name, int transactionID, int transactionTimeStamp);
	};
};

#endif // __STORE_NAMEDROOTS_H__
