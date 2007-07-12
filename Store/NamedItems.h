#ifndef __NAMEDITEMS_H__
#define __NAMEDITEMS_H__

namespace Store
{
	class Classes;
};

#define IX_DEBUG	1

#define STORE_IXC_INITIALPAGECOUNT		2
#define STORE_IXC_NAMEMAXLEN			1000
#define STORE_IXC_NULLVALUE				-1


#include <stdlib.h>
#include <string.h>
#include <vector>

#include "Struct.h"
#include "File.h"
#include "Buffer.h"
#include "PagePointer.h"
#include "Log/Logs.h"
#ifdef IXR_DEBUG
#include "Errors/ErrorConsole.h"
#endif

using namespace std;
using namespace Logs; 

namespace Store
{
	class NamedItems
	{
	protected: 
		unsigned int STORE_IX_INITIALPAGECOUNT;
		int STORE_IX_NAMEMAXLEN;
		int STORE_IX_NULLVALUE;
		int STORE_FILE_;
		int STORE_PAGE_HEADER;
		int STORE_PAGE_PAGE;
	private:
		typedef struct ixr_header
		{
			page_header page_hdr;
		} ixr_header;

		typedef struct ixr_page
		{
			page_header page_hdr;
			int free;
			int entries;
		} ixr_page;

		typedef struct ixr_entry
		{
			int size;
			int l_id;
			int cur_tran;
			int add_t;
			int del_t;
			char name[];
		} ixr_entry;

		Buffer* buffer;
		LogManager* log;
#ifdef IXR_DEBUG
		ErrorConsole* ec;
#endif

		int modifyTransaction(int transactionID, int mode);

	public:
		NamedItems();
		~NamedItems();

		void init(Buffer* buffer, LogManager* log);
		int initializeFile(File* file);
		int initializePage(unsigned int pageID, char* page);

		int commitTransaction(int transactionID);
		int abortTransaction(int transactionID);
		int abortAllTransactions();

		int addItem(int logicalID, const char* name, int transactionID, int transactionTimeStamp);
		int removeItem(int logicalID, int transactionID, int transactionTimeStamp);
		vector<int>* getItems(int transactionID, int transactionTimeStamp);
		vector<int>* getItems(const char* name, int transactionID, int transactionTimeStamp);

	};
	
	class Classes : public NamedItems {
	public:
		Classes() {
			STORE_IX_INITIALPAGECOUNT = STORE_IXC_INITIALPAGECOUNT;
			STORE_IX_NAMEMAXLEN = STORE_IXC_NAMEMAXLEN;
			STORE_IX_NULLVALUE = STORE_IXC_NULLVALUE;
			STORE_FILE_ = STORE_FILE_CLASSES;
			STORE_PAGE_HEADER = STORE_PAGE_CLASSESHEADER;
			STORE_PAGE_PAGE = STORE_PAGE_CLASSESPAGE;
		}		
	};

};


#endif /*NAMEDITEMS_H_*/
