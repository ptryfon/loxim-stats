/*
 * AbstractStats.h
 *
 *  Created on: 18-Nov-09
 *      Author: Bartosz Borkowski
 */

#ifndef ABSTRACTSTATS_H_
#define ABSTRACTSTATS_H_

#include <string>
#include <vector>

namespace SystemStatsLib{

	#define EMPTY_STATS_ID						0
	#define CONFIG_STATS_ID					1
	#define QUERIES_STATS_ID					2
	#define SESSION_STATS_ID				3
	#define STORE_STATS_ID						4
	#define TRANSACTIONS_STATS_ID	5

	#define EMPTY_STATS_OUTPUT					\
		StatsOutput *ret = new StatsOutput;	\
		ret->stats_id = EMPTY_STATS_ID;			\
		return ret;

	typedef struct StatsOutput {
		unsigned int stats_id;
		std::vector<int> int_stats;
		std::vector<std::string> int_names;
		std::vector<double> double_stats;
		std::vector<std::string> double_names;
		std::vector<std::string> string_stats;
		std::vector<std::string> string_names;
	};

	class AbstractStats {
		public:
			AbstractStats(const std::string& _name) : name(_name) {}
			virtual ~AbstractStats() {}
			const std::string& get_name() const {return name;}
			virtual StatsOutput * get_stats_output() const = 0;
		private:
			std::string name;
	};

}

#endif /* ABSTRACTSTATS_H_ */
