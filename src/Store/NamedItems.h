#ifndef __NAMEDITEMS_H__
#define __NAMEDITEMS_H__

namespace Store
{
	class Classes;
	class Interfaces;
	class StoreSchemas;
	class NamedRoots;
	class Buffer;
	class File;
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

#include <Store/Struct.h>
#include <Log/Logs.h>
#include <Indexes/RootEntry.h>
#include <TransactionManager/Transaction.h>
#ifdef IXR_DEBUG
#include <Errors/ErrorConsole.h>
#endif

using namespace std;
using namespace Logs;
using namespace TManager;

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
	
	struct ixi_entry : public ix_entry {
		string getName() const {
		    string nameS(name); 
		    return nameS;
		}
	
		string getObjectName() const {
			string oName(name + strlen(name) + 1);
			return oName;
		}
		string getBindName() const {
			string bName(name + strlen(name) + 1 + strlen(name + strlen(name) + 1) + 1);
			return bName;
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
	
	struct FindByObjectName {	//to avoid confusion
		bool operator() (const char* oName, ix_entry* entry) {
		    return (strcmp(oName, (static_cast<ixi_entry*>(entry))->getObjectName().c_str()) == 0);
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
		vector<int>* getByNameWithEntries(TransactionID* tid, const char* name, vector<const ix_entry *> *entries = NULL);
	    
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
		vector<int>* getItemsByAnything(TransactionID* tid, Operation findTest, DataType thingToFind, vector<Indexes::RootEntry*> *indexContent = NULL, vector<const ix_entry *> *entryNames = NULL);

	};

	class Interfaces : public NamedItems {
	private:
	    ErrorConsole *ec;
	    static FindByObjectName findByObjectName;
	    int createEntry(TransactionID* tid, int logicalID, const char* name, const char* objectName, int& size_needed, char*& entry_buf, const char* bindName = NULL);
	public:
	    ~Interfaces();
	    Interfaces();
	    int addInterface(TransactionID* tid, int logicalID, const string& name, const string& objectName, const string& bindName = "");
	    int bindInterface(TransactionID* tid, const string& name, const string& bindName);
	    vector<int>* getInterfaceByObjectName(TransactionID* tid, const char* oName, vector<const ix_entry *> *entries = NULL);
	    vector<int>* getInterfaceByName(TransactionID* tid, const char* name, vector<const ix_entry *> *entries = NULL);
	    int getInterfaceBindForObjectName(TransactionID* tid, const string& oName, string& interfaceName, string& bindName);
	    int getInterfaceBindForName(TransactionID* tid, const string& name, string& interfaceName, string& bindName);
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
			this->ec = &ErrorConsole::get_instance(EC_STORE_CLASSES);
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

	class StoreSchemas : public NamedItems 
	{
		private:
			ErrorConsole &ec;
			
		public:
			StoreSchemas();
			~StoreSchemas() {}
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
			this->ec = &ErrorConsole::get_instance(EC_STORE_NAMED_ROOTS);
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
