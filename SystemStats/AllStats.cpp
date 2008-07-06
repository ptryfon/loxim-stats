/*
 * AllStats.cpp
 *
 *  Created on: 2-lip-08
 *      Author: damianklata
 */

#include "AllStats.h"

using namespace SystemStatsLib;

AllStats::AllStats(): SystemStats("ALL_STATS") {
	setStatsStats("SESSIONS_STATS", new SessionsStats());
	setStatsStats("CONFIGS_STATS", new ConfigsStats());
}

AllStats* AllStats::allStats = NULL;//new TransactionManager();

AllStats* AllStats::getHandle() {
	if (allStats == NULL) {
		allStats = new AllStats();
	}
	return allStats;
}

SessionsStats* AllStats::getSessionsStats() {
	return dynamic_cast<SessionsStats*>(getStatsStats("SESSIONS_STATS"));
}

ConfigsStats* AllStats::getConfigsStats() {
	return dynamic_cast<ConfigsStats*>(getStatsStats("CCONFIGS_STATS"));
}

AllStats::~AllStats() {
}
