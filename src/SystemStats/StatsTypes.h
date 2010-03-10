#ifndef STATS_TYPES_H
#define STATS_TYPES_H

namespace SystemStatsLib {

enum StatsScope {
	STORE = 1,
	SESSION = 2,
	TRANSACTION = 4,
	QUERIES = 8,
	CONFIG = 16,
	ALL = 31,
};

} //namespace SystemStatsLib

#endif //STATS_TYPES_H
