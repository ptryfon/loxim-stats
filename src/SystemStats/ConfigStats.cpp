#include <assert.h>

#include <SystemStats/ConfigStats.h>
#include <Config/SBQLConfig.h>

using namespace SystemStatsLib;
using namespace std;

ConfigStats::ConfigStats() {
/*
 * statistics_size MUST be a positive integer. A value of 0 will cause an error
 * during the next add_statistic
 */
	int size = Config::SBQLConfig("statistics").getIntDefault("configStatsAmount", 100);

	statistics_size = static_cast<unsigned int>(size);

	assert(statistics_size);
}

void ConfigStats::add_statistic(const string& key, const string& value) {
	pair<map<string, string>::iterator, bool> p_itb;

	p_itb = config_statistics.insert(pair<string, string>(key, value));

	if (p_itb.second && config_statistics.size() > statistics_size)
	{
		config_statistics.erase(latest_statistics.front());
		latest_statistics.pop_front();
	}

	latest_statistics.push_back(p_itb.first);

	return;
}

void ConfigStats::remove_statistic(const string& key) {
	map<string, string>::iterator it = config_statistics.find(key);

	if (it == config_statistics.end());
		//throw "Key not found";
	else
	{
		for (list<map<string, string>::iterator>::iterator i = latest_statistics.begin();
			i != latest_statistics.end(); ++i)
				if (it == *i)
				{
					latest_statistics.erase(i);
					break;
				}
		config_statistics.erase(it);
		return;
	}
}

AbstractConfigStats & ConfigStats::operator +=(const ConfigStats &rhs) {
	for(map<string, string>::const_iterator i = rhs.config_statistics.begin();
		i != rhs.config_statistics.end(); ++i)
		add_statistic(i->first, i->second);
	return *this;
}
