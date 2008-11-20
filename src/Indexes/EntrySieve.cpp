#include "EntrySieve.h"

#include "Store/DBLogicalID.h"
#include "IndexManager.h"

using namespace Store;
namespace Indexes
{

	bool INDEX_SIEVE_DEBUG = false;

	EntrySieve::EntrySieve(TransactionID* tid, lid_t index)
		: result(new sieveContent_t()), tid(tid), started(false), index(index) {}
	
	EntrySieve::~EntrySieve(){}
		
	//w loximie jest blad. storowe ixr_entry ma add_t int a timestamp z TransactionID jest unsigned i dane te sa porownywane. musi byc signed bo wpisy w storze sa -1?
	
	void EntrySieve::stateReset(RootEntry* entry) {
		currentLid = entry->logicalID;
		latestTimestamp = 0;
		
	}
	
	bool EntrySieve::isEntryVisible(RootEntry* entry) {
		
		if (entry->modyfierTransactionId == ROLLED_BACK) {
			return false;
		}
		
		if (entry->modyfierTransactionId == tid->getId()) {
			return true;
		}
		
		if (!((entry->addTime) <= (ts_t) tid->getTimeStamp())) {
			return false;
		}
		
		if (!entry->delTime == STORE_IXR_NULLVALUE) {
			return false;
		}
		//zaczyna sie OR
		if (entry->modyfierTransactionId == STORE_IXR_NULLVALUE) {
			return true;
		}
		
		if (indexVisible(entry)) {
			return true;
		}
		
		return false;
		
		return (((unsigned)entry->addTime) <= tid->getTimeStamp())
						&& (entry->delTime == STORE_IXR_NULLVALUE)
						&& (entry->modyfierTransactionId == STORE_IXR_NULLVALUE || entry->modyfierTransactionId == tid->getId() || 
								  indexVisible(entry)
							);
	}
	
	void EntrySieve::add(RootEntry* entry) {
		
		if (INDEX_SIEVE_DEBUG) {
			printf("sieve: %s\n", entry->toString().c_str());
			printf("tid: TransactionID@[timeStamp= %d]\n", tid->getTimeStamp());
		}
		
		/*if ()
		
		if (entry->delTime == tid->getTimeStamp() && lastRemoved < ) {
			removed = true;
		}*/
		
		if (isEntryVisible(entry)) {
		
			//zmiany sa widoczne w tej transakcji
			if (!started) {
				started = true;
				stateReset(entry);
			}
			
			if (currentLid != entry->logicalID) {
				pushLatest();
				stateReset(entry);
			}
		
			if (entry->addTime > latestTimestamp) {
				latestTimestamp = entry->addTime;
				removed = false;
			}
			
			if (entry->delTime > latestTimestamp) {
				latestTimestamp = entry->delTime;
				removed = true;
			}	
			
		}
	}
	
	void EntrySieve::pushLatest() {
		if (removed) {
			return;
		}
		result->push_back( new QueryReferenceResult( new DBLogicalID(currentLid) ) );
	}
	
	bool EntrySieve::indexVisible(RootEntry* entry) {
		return IndexManager::getHandle()->resolver->isVisible(entry->modyfierTransactionId, tid->getTimeStamp(), index);
	}
	
	sieveContent_t* EntrySieve::getResult() {
		if (started) {
			pushLatest();
		}
		return result.release();
	}
}
