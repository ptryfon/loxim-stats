/*
 * AllStats.h
 *
 *  Created on: 2-lip-08
 *      Author: damianklata
 */

#ifndef ALLSTATS_H_
#define ALLSTATS_H_

#include <SystemStats/SystemStats.h>
#include <SystemStats/SessionStats.h>
#include <SystemStats/ConfigStats.h>
#include <SystemStats/StoreStats.h>
#include <SystemStats/QueriesStats.h>
#include <SystemStats/TransactionsStats.h>

using namespace std;


namespace SystemStatsLib{
/*
 * This is class that collect all statistics that
 * we use in database. We can access to this class
 * in static way (getHandle method). Through this
 * class we have access from others modules.
 */
	class AllStats: public SystemStats{
	protected:
			static AllStats* allStats;
			public:
				AllStats();

				static AllStats* getHandle();

				SessionsStats* getSessionsStats();
				ConfigsStats* getConfigsStats();
				StoreStats* getStoreStats();
				TransactionsStats* getTransactionsStats();
				QueriesStats* getQueriesStats();

				void addDiskPageReads(int sessionId, int transactionId, int count);
				void addPageReads(int sessionId, int transactionId, int count);
				void addDiskPageWrites(int sessionId, int transactionId, int count);
				void addPageWrites(int sessionId, int transactionId, int count);

				~AllStats();
		};
}

#endif /* ALLSTATS_H_ */
