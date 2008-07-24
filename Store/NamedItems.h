#ifndef __NAMEDITEMS_H__
#define __NAMEDITEMS_H__

namespace Store
{
	class Classes;
	class Interfaces;
	class NamedRoots;
};

#define IXR_DEBUG	1
#define IX_DEBUG	1

#define STORE_IXR_INITIALPAGECOUNT		2
#define STORE_IXR_NAMEMAXLEN			100
#define STORE_IXR_NULLVALUE				-1

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
#include "Indexes/RootEntry.h"
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

	struct FindNamesWithBegin {
		bool operator()(const char* nameBegin, ix_entry* entry) {
			//cout << "********************** ------------------> ";
			//cout << (static_cast<ixc_entry*>(entry))->getInvariant() << endl;
			return (strstr(entry->name, nameBegin) == entry->name);
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

		virtual int createEntry(TransactionID* tid, int logicalID, const char* name, int& size_needed, char*& entry_buf);
		virtual int addItem(TransactionID* tid, int size_needed, char* entry_buf);

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

		int modifyTransaction(TransactionID* tid, int mode);

	public:
		NamedItems();
		virtual ~NamedItems();

		void init(Buffer* buffer, LogManager* log);
		int initializeFile(File* file);
		int initializePage(unsigned int pageID, char* page);

		int commitTransaction(TransactionID* tid);
		int abortTransaction(TransactionID* tid);
		int abortAllTransactions();

		int addItem(TransactionID* tid, int logicalID, const char* name);
		int removeItem(TransactionID* tid, int logicalID);
		vector<int>* getItems(TransactionID* tid);
		vector<int>* getItems(TransactionID* tid, const char* name, vector<Indexes::RootEntry*> *indexContent = NULL);

		template<typename Operation, typename DataType>
		vector<int>* getItemsByAnything(TransactionID* tid, Operation findTest, DataType thingToFind, vector<Indexes::RootEntry*> *indexContent = NULL);

	};

	class Interfaces : public NamedItems {
	private:
	    ErrorConsole *ec;
	    int createEntry(int logicalID, const char* name, const char* objectName, int transactionID, int transactionTimeStamp, int& size_needed, char*& entry_buf);
	public: 
	    ~Interfaces();
	    Interfaces();
	    int addInterface(int logicalID, const char* name, const char* objectName, int transactionID, int transactionTimeStamp);
	};

	class Classes : public NamedItems {
	private:
#ifdef IXR_DEBUG
		ErrorConsole* ec;
#endif

	protected:

		static FindByInvariantName findByInvariantName;

		int invaraint_maxlen;
		virtual int createEntry(TransactionID* tid, int logicalID, const char* name, int& size_needed, char*& entry_buf);
		virtual int createEntry(TransactionID* tid, int logicalID, const char* name, const char* invariantName, int& size_needed, char*& entry_buf);
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

		int addClass(TransactionID* tid, int logicalID, const char* name, const char* invariantName);

		vector<int>* getClassByInvariant(TransactionID* tid, const char* invariantName);
	};

	class NamedRoots : public NamedItems {
	private:
#ifdef IXR_DEBUG
		ErrorConsole* ec;
#endif

	protected:

		static FindNamesWithBegin findNamesWithBegin;

	public:
		~NamedRoots();
		NamedRoots() {
#ifdef IX_DEBUG
			this->ec = new ErrorConsole("Store: NamedRoots");
#endif
			STORE_IX_INITIALPAGECOUNT = STORE_IXR_INITIALPAGECOUNT;
			STORE_IX_NAMEMAXLEN = STORE_IXR_NAMEMAXLEN;
			STORE_IX_NULLVALUE = STORE_IXR_NULLVALUE;
			STORE_FILE_ = STORE_FILE_ROOTS;
			STORE_PAGE_HEADER = STORE_PAGE_ROOTSHEADER;
			STORE_PAGE_PAGE = STORE_PAGE_ROOTSPAGE;
		}

		int addRoot(TransactionID* tid, int logicalID, const char* name) {
			return addItem(tid, logicalID, name);
		}
		int removeRoot(TransactionID* tid, int logicalID) {
			return removeItem(tid, logicalID);
		}
		vector<int>* getRoots(TransactionID* tid) {
			return getItems(tid);
		}
		vector<int>* getRoots(TransactionID* tid, const char* name, vector<Indexes::RootEntry*> *indexContent = NULL) {
			return getItems(tid, name, indexContent);
		}
		vector<int>* getRootsWithBegin(TransactionID* tid, const char* nameBegin);
	};

};


#endif /*NAMEDITEMS_H_*/
