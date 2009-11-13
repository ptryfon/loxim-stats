/*
 * ConfigStats.h
 *
 *  Created on: 6-Nov-09
 *      Author: Bartosz Borkowski
 */

#ifndef CONFIGSTATS_H_
#define CONFIGSTATS_H_

using namespace std;

namespace SystemStatsLib {

	class AbstractConfigsStats {
		public:
			virtual ~AbstactConfigsStats() {}
			virtual void add_statistic(const string& key, const string& value) = 0;
			virtual void remove_statistic(const string& key) = 0;
	};

	class ConfigsStats : public AbstactConfigStats {
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

		void add_statistic(const string& key, const string& value);
		void remove_statistic(const string& key);
	private:
		unsigned statistics_size;

		map<string, string> configs_statistics;
		list<map<string, string>::iterator> latest_statitics;
	};

	class EmptyConfigsStats : public AbstactConfigsStats {
		public:
			void add_statistic(const string&, const string&) {}
			void remove_statistic(const string& key) {}
	};
}

#endif /* CONFIGSTATS_H_ */
