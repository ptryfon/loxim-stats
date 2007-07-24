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
	
	struct ix_entry {
		int size;
		int l_id;
		int cur_tran;
		int add_t;
		int del_t;
		char name[];
	};
	
	struct ixc_entry : public ix_entry {
		char* getInvariant() {
			return name + strlen(name) + 1;
		}
	};

	struct FindByName {
		bool operator()(const char* name, ix_entry* entry) {
			return (strlen(name) == 0 || strcmp(name, entry->name) == 0);
		}
	};
	
	struct FindByInvariantName {
		bool operator()(const char* invariant, ix_entry* entry) {
			//cout << "********************** ------------------> ";
			//cout << (static_cast<ixc_entry*>(entry))->getInvariant() << endl;
			return (strcmp(invariant, (static_cast<ixc_entry*>(entry))->getInvariant()) == 0);
		}
	};
	
	class NamedItems
	{
	protected: 
		unsigned int STORE_IX_INITIALPAGECOUNT;
		int STORE_IX_NAMEMAXLEN;
		int STORE_IX_NULLVALUE;
		int STORE_FILE_;
		int STORE_PAGE_HEADER;
		int STORE_PAGE_PAGE;
		
		virtual int createEntry(int logicalID, const char* name, int transactionID, int transactionTimeStamp, int& size_needed, char*& entry_buf);
		virtual int addItem(int size_needed, char* entry_buf);
		
		static FindByName findByName;
		
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


		Buffer* buffer;
		LogManager* log;
#ifdef IXR_DEBUG
		ErrorConsole* ec;
#endif

		int modifyTransaction(int transactionID, int mode);

	public:
		NamedItems();
		virtual ~NamedItems();

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
		
		template<typename Operation, typename DataType>
		vector<int>* getItemsByAnything(Operation findTest, DataType thingToFind, int transactionID, int transactionTimeStamp);
		
	};
	
	class Classes : public NamedItems {
	private:
#ifdef IXR_DEBUG
		ErrorConsole* ec;
#endif
	
	protected:
		
		static FindByInvariantName findByInvariantName;
		
		int invaraint_maxlen;
		virtual int createEntry(int logicalID, const char* name, int transactionID, int transactionTimeStamp, int& size_needed, char*& entry_buf);
		virtual int createEntry(int logicalID, const char* name, const char* invariantName, int transactionID, int transactionTimeStamp, int& size_needed, char*& entry_buf);
	public:
		~Classes();
		Classes() {
#ifdef IX_DEBUG
			this->ec = new ErrorConsole("Store: Classes");
#endif
			STORE_IX_INITIALPAGECOUNT = STORE_IXC_INITIALPAGECOUNT;
			STORE_IX_NAMEMAXLEN = STORE_IXC_NAMEMAXLEN;
			STORE_IX_NULLVALUE = STORE_IXC_NULLVALUE;
			STORE_FILE_ = STORE_FILE_CLASSES;
			STORE_PAGE_HEADER = STORE_PAGE_CLASSESHEADER;
			STORE_PAGE_PAGE = STORE_PAGE_CLASSESPAGE;
			invaraint_maxlen = STORE_IXR_NAMEMAXLEN;
		}
		
		int addClass(int logicalID, const char* name, const char* invariantName, int transactionID, int transactionTimeStamp);

		vector<int>* getClassByInvariant(const char* invariantName, int transactionID, int transactionTimeStamp);
	};

};


#endif /*NAMEDITEMS_H_*/
