#ifdef ___NODEFFORIT___
/*
 * Uwaga ten plik zostal wylaczony specjalnie.
 */

//#ifndef __STORE_IXR_H__
#define __STORE_IXR_H__

#define IXR_DEBUG	1

#define STORE_IXR_INITIALPAGECOUNT		2
#define STORE_IXR_NAMEMAXLEN			100
#define STORE_IXR_NULLVALUE				-1

namespace Store
{
	class NamedRoots;
};

#include <stdlib.h>
#include <string.h>
#include <vector>

#include <Store/Struct.h>
#include <Store/File.h>
#include <Store/Buffer.h>
#include <Store/PagePointer.h>
#include <Log/Logs.h>
#ifdef IXR_DEBUG
#include <Errors/ErrorConsole.h>
#endif

using namespace std;
using namespace Logs; 

namespace Store
{
	class NamedRoots
	{
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
		NamedRoots();
		~NamedRoots();

		void init(Buffer* buffer, LogManager* log);
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

#endif // __STORE_IXR_H__
