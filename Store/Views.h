#ifndef __STORE_IXV_H__
#define __STORE_IXV_H__

#define IXV_DEBUG	1

#define STORE_IXV_INITIALPAGECOUNT		2
#define STORE_IXV_NAMEMAXLEN			100
#define STORE_IXV_NULLVALUE				-1

namespace Store
{
	class Views;
};

#include <stdlib.h>
#include <string.h>
#include <vector>

#include "Struct.h"
#include "File.h"
#include "Buffer.h"
#include "PagePointer.h"
#include "Log/Logs.h"
#ifdef IXV_DEBUG
#include "Errors/ErrorConsole.h"
#endif

using namespace std;
using namespace Logs; 

namespace Store
{
	class Views
	{
	private:
		typedef struct ixv_header
		{
			page_header page_hdr;
		} ixv_header;

		typedef struct ixv_page
		{
			page_header page_hdr;
			int free;
			int entries;
		} ixv_page;

		typedef struct ixv_entry
		{
			int size;
			int l_id;
			int cur_tran;
			int add_t;
			int del_t;
			char name[];
		} ixv_entry;

		Buffer* buffer;
		LogManager* log;
#ifdef IXV_DEBUG
		ErrorConsole* ec;
#endif

		int modifyTransaction(int transactionID, int mode);

	public:
		Views();
		~Views();

		void init(Buffer* buffer, LogManager* log);
		int initializeFile(File* file);
		int initializePage(unsigned int pageID, char* page);

		int commitTransaction(int transactionID);
		int abortTransaction(int transactionID);
		int abortAllTransactions();

		int addView(int logicalID, const char* name, int transactionID, int transactionTimeStamp);
		int removeView(int logicalID, int transactionID, int transactionTimeStamp);
		vector<int>* getViews(int transactionID, int transactionTimeStamp);
		vector<int>* getViews(const char* name, int transactionID, int transactionTimeStamp);
	};
};

#endif // __STORE_IXV_H__