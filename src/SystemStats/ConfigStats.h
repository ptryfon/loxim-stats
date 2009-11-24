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

	class AbstractConfigsStats : public AbstractStats {
		public:
			AbstractConfigsStats() : AbstractStats("CONFIGS_STATS") {}
			virtual ~AbstractConfigsStats() {}
			virtual void add_statistic(const std::string& key, const std::string& value);
			virtual void remove_statistic(const std::string& key);
	};

	class ConfigsStats : public AbstractConfigsStats {
	/*
	 * Statistics' keys are kept in a format:
	 * key: 'module_name'_'option_name'
	 * value: 'option_value'
	 * Until the SBQLConfig changes this will have to do
	 * In an effort to reduce memory usage, only statistics_size latest statistics
	 * will be stored.
	 */
	public:
		ConfigsStats();

		void add_statistic(const std::string& key, const std::string& value);
		void remove_statistic(const std::string& key);
	private:
		unsigned int statistics_size;

		std::map<std::string, std::string> configs_statistics;
		std::list<std::map<std::string, std::string>::iterator> latest_statistics;
	};

	class EmptyConfigsStats : public AbstractConfigsStats {
		public:
			void add_statistic(const std::string&, const std::string&) {}
			void remove_statistic(const std::string& key) {}
	};
}

#endif /* CONFIGSTATS_H_ */
