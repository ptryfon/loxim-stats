/*
 * AllStats.h
 *
 *  Created on: 2-lip-08
 *      Author: damianklata
 */

#ifndef ALLSTATS_H_
#define ALLSTATS_H_

#include "SystemStats.h"
#include "SessionStats.h"
#include "ConfigStats.h"

using namespace std;


namespace SystemStatsLib{
	class AllStats: public SystemStats{
	protected:
			static AllStats* allStats;
			public:
				AllStats();

				static AllStats* getHandle();

				SessionsStats* getSessionsStats();
				ConfigsStats* getConfigsStats();

				void addDiskPageReads(int sessionId, int transactionId, int count);
				void addPageReads(int sessionId, int transactionId, int count);
				void addDiskPageWrites(int sessionId, int transactionId, int count);
				void addPageWrites(int sessionId, int transactionId, int count);

				~AllStats();
		};
}

#endif /* ALLSTATS_H_ */
