/*
 * ConfigStats.h
 *
 *  Created on: 6-Nov-09
 *      Author: Bartosz Borkowski
 */

#ifndef CONFIGSTATS_H_
#define CONFIGSTATS_H_

#include <list>
#include<map>

#include <SystemStats/AbstractStats.h>

namespace SystemStatsLib {

	class AbstractConfigStats;
	class ConfigStats;
	class EmptyConfigStats;
	/* Forward declarations.
	 */

	class AbstractConfigStats : public AbstractStats {
		public:
			AbstractConfigStats() : AbstractStats("CONFIGS_STATS") {}
			virtual ~AbstractConfigStats() {}
			virtual void add_statistic(const std::string& key, const std::string& value) = 0;
			virtual void remove_statistic(const std::string& key) = 0;
			virtual AbstractConfigStats & operator +=(const ConfigStats &rhs) = 0;
			AbstractConfigStats & operator +=(const EmptyConfigStats &) {return *this;}
	};

	class ConfigStats : public AbstractConfigStats {
	/*
	 * Statistics' keys are kept in a format:
	 * key: 'module_name'_'option_name'
	 * value: 'option_value'
	 * Until the SBQLConfig changes this will have to do
	 * In an effort to reduce memory usage, only statistics_size latest statistics
	 * will be stored.
	 */
	public:
		ConfigStats();

		void add_statistic(const std::string& key, const std::string& value);
		void remove_statistic(const std::string& key);

		AbstractConfigStats & operator +=(const ConfigStats &rhs);
		StatsOutput * get_stats_output() const;
	private:
		unsigned int statistics_size;

		std::map<std::string, std::string> config_statistics;
		std::list<std::map<std::string, std::string>::iterator> latest_statistics;
	};

	class EmptyConfigStats : public AbstractConfigStats {
		public:
			~EmptyConfigStats() {}

			void add_statistic(const std::string&, const std::string&) {}
			void remove_statistic(const std::string& key) {}

			AbstractConfigStats & operator +=(const ConfigStats &) {return *this;}
			StatsOutput * get_stats_output() const {EMPTY_STATS_OUTPUT}
	};
}

#endif /* CONFIGSTATS_H_ */
