#ifndef ENTRYSIEVE_H_
#define ENTRYSIEVE_H_

#include <TransactionManager/Transaction.h>
#include <QueryExecutor/QueryResult.h>
#include <vector>

using namespace std;
using namespace QExecutor;

namespace Indexes
{
	
	typedef vector<QueryResult*> sieveContent_t;
	
	class EntrySieve {
		protected:
			auto_ptr<sieveContent_t> result;
			TransactionID* tid;
			bool started;
			lid_t currentLid;
			ts_t latestTimestamp;
			bool removed;
			lid_t index;
			
			bool indexVisible(RootEntry* entry);
			void stateReset(RootEntry* entry);
			void pushLatest();
			bool isEntryVisible(RootEntry* entry);
			
			//auto_ptr<ErrorConsole> ec;
			
		public:
			/** destruktor EntrySieve nie niszczy obiektu Transaction */
			EntrySieve(TransactionID* tid, lid_t index);
			virtual ~EntrySieve();
						
			/** zwraca wynik zapytania wlasciwy dla transakcji */
			virtual sieveContent_t* getResult();
			
			/** dodaje kolejny LID z RootEntry o ile powinien on byc widoczny dla transakcji */
			virtual void add(RootEntry* entry);
	};
	
}

#endif /*ENTRYSIEVE_H_*/
