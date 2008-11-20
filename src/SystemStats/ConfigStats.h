/*
 * ConfigStats.h
 *
 *  Created on: 5-lip-08
 *      Author: damianklata
 */

#ifndef CONFIGSTATS_H_
#define CONFIGSTATS_H_
#include <SystemStats/SystemStats.h>

using namespace std;

namespace SystemStatsLib {
/*
 * Configs stats is the simples stats that show
 * only configuration file readed from disk.
 */
	class ConfigOptStats: public SystemStats {

	public:
		ConfigOptStats();

		void setKey(string value);
		string getKey();

		void setValue(string value);
		string getValue();

		~ConfigOptStats();
	};

	class ConfigModuleStats: public SystemStats {

	public:
		ConfigModuleStats();

		void setModuleName(string value);
		string getModuleName();

		void addConfigOptStats(string key, ConfigOptStats* value);
		ConfigOptStats* getConfigOptStats(string key);
		void removeConfigOptStats(string key);

		~ConfigModuleStats();
	};

	class ConfigsStats: public SystemStats {

	public:
		ConfigsStats();

		void addConfigModuleStats(string key, ConfigModuleStats* value);
		ConfigModuleStats* getConfigModuleStats(string key);
		void removeConfigModuleStats(string key);

		~ConfigsStats();
	};
}

#endif /* CONFIGSTATS_H_ */
