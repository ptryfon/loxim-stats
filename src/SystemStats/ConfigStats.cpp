#include <SystemStats/ConfigStats.h>
#include <Config/SBQLConfig.h>

using namespace SystemStatsLib;
/* =-=-=-=-=-=-=-=-=-=- */

ConfigsStats::ConfigsStats()
/*
 * statistics_size MUST be a positive integer. A value of 0 will cause an error
 * during the next add_statistic
 */
{
	SBQLConfig("statistics").getInt("configStatsAmount", statistic_size);

	assert(statistics_size);
}

void ConfigsStats::add_statistc(const string& key, const string& value)
{
	pair<map<string, string>::iterator, bool> p_itb;

	p_itb = configs_statistics.insert(pair<string, string>(key, value));

	if (p_itb.second && configs_statistics.size() > statistic_size)
	{
		configs_statistics.erase(latest_statistics.front());
		latest_statistics.pop_front();
	}

	latest_statistics.push_back(p_itb.first);

	return;
}

void ConfigsStats::remove_statistic(const string& key)
{
	map<string, string>::iterator it = configs_statistics.find(key);

	if (it == configs_statistics.end())
		throw "Key not found";
	else
	{
		latest_statistics.erase(latest_statistics.find(it));
		configs_statistics.erase(it);
		return;
	}
}
