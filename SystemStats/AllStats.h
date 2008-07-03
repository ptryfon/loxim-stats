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

using namespace std;


namespace SystemStatsLib{
	class AllStats: public SystemStats{
	protected:
			static AllStats* allStats;
			public:
				AllStats();

				static AllStats* getHandle();

				SessionsStats* getSessionsStats();

				~AllStats();
		};
}

#endif /* ALLSTATS_H_ */
