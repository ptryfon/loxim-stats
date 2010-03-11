/*
 * Statistics.cpp
 *
 *  Created on: 12-Nov-09
 *      Author: Bartosz Borkowski
 */

#include <SystemStats/Statistics.h>
#include <Config/SBQLConfig.h>

#include <iostream>

using namespace SystemStatsLib;

pthread_t Statistics::root = 0;
std::map<pthread_t, StatisticSingleton*> Statistics::statistic_singletons =
	std::map<pthread_t, StatisticSingleton*>();
Util::Mutex Statistics::singleton_mutex;

StatisticSingleton::StatisticSingleton() {
/* 
 * I really, really hope that SBQLConfig works this way.
 * Beacause if not than it's not possible to do it in a
 * nice way. We open the config to see which statistics
 * we want active and which we don't want.
 */

   Config::SBQLConfig config("statistics");

	if (config.getBoolDefault("sessionStats", true))
		active_sessions_stats = &sessions_stats;
	else
		active_sessions_stats = &empty_sessions_stats;

	if (config.getBoolDefault("configStats", true))
		active_config_stats = &config_stats;
	else
		active_config_stats = &empty_config_stats;

	if (config.getBoolDefault("storeStats", true))
		active_store_stats = &store_stats;
	else
		active_store_stats = &empty_store_stats;

	if (config.getBoolDefault("transactionsStats", true))
		active_transactions_stats = &transactions_stats;
	else
		active_transactions_stats = &empty_transactions_stats;

	if (config.getBoolDefault("queriesStats", true))
		active_queries_stats = &queries_stats;
	else
		active_queries_stats = &empty_queries_stats;
}

AbstractStats& StatisticSingleton::get_abstract_stats(const string& name) {
	if (name == "SESSIONS_STATS")
		return *active_sessions_stats;
	else if (name == "STORE_STATS")
		return *active_store_stats;
	else if (name == "CONFIG_STATS")
		return *active_config_stats;
	else if (name == "TRANSACTIONS_STATS")
		return *active_transactions_stats;
	else if (name =="QUERIES_STATS")
		return *active_queries_stats;
	else
		throw ("Unrecognised stats name");
}

void StatisticSingleton::addDiskPageReads(int sessionId, int /*transactionId*/, int count) {
// 	active_sessions_stats->add_disk_page_reads(sessionId, count);
// 	active_store_stats->add_disk_page_reads(count);
// 	active_queries_stats->add_disk_io(sessionId, count);
}

void StatisticSingleton::addPageReads(int sessionId, int /*transactionId*/, int count) {
// 	active_sessions_stats->add_page_reads(sessionId, count);
// 	active_store_stats->add_page_reads(count);
}

void StatisticSingleton::addDiskPageWrites(int sessionId, int /*transactionId*/, int count) {
// 	active_sessions_stats->add_disk_page_writes(sessionId, count);
// 	active_store_stats->add_disk_page_writes(count);
// 	active_queries_stats->add_disk_io(sessionId, count);
}

void StatisticSingleton::addPageWrites(int sessionId, int /*transactionId*/, int count) {
// 	active_sessions_stats->add_page_writes(sessionId, count);
// 	active_store_stats->add_page_writes(count);
}

StatisticSingleton& Statistics::get_statistics(const pthread_t thread_id) {
	Util::Mutex::MutexLocker locker(singleton_mutex);
	if (statistic_singletons.empty())
		root = thread_id;

	pair<map<pthread_t, StatisticSingleton*>::iterator, bool> tmp;

	tmp = statistic_singletons.insert(
		pair<pthread_t, StatisticSingleton*>(thread_id, new StatisticSingleton()));

	return *tmp.first->second;
}

#define unified_stats(stats_name, get_stats) {												\
	stats_name *s = new stats_name;															\
	for (map<pthread_t, StatisticSingleton*>::iterator i = statistic_singletons.begin();	\
		i != statistic_singletons.end(); ++i)												\
		(*s) += i->second->get_stats();														\
	return s;																				\
	}

AbstractStats* Statistics::get_unified_statistics(const pthread_t thread_id,
	const string &name) {
	Util::Mutex::MutexLocker locker(singleton_mutex);

	if (thread_id != root)
		throw (0);

	if (name == "SESSIONS_STATS")
		unified_stats(SessionsStats, get_c_sessions_stats)
	else if (name == "STORE_STATS")
		unified_stats(StoreStats, get_c_store_stats)
	else if (name == "CONFIG_STATS")
		unified_stats(ConfigStats, get_c_config_stats)
	else if (name == "TRANSACTIONS_STATS")
		unified_stats(TransactionsStats, get_c_transactions_stats)
	else if (name =="QUERIES_STATS")
		unified_stats(QueriesStats, get_c_queries_stats)
	else
		throw ("Unrecognised stats name");
}

AbstractStats* 
Statistics::get_unified_statistics(StatsScope scope)
{
	switch (scope) {
		case STORE:
			return Statistics::get_unified_statistics(pthread_t get_root(), "STORE_STATS");
			break;
		case SESSION:
			return Statistics::get_unified_statistics(pthread_t get_root(), "SESSIONS_STATS");
			break;
		case TRANSACTION:
			return Statistics::get_unified_statistics(pthread_t get_root(), "TRANSACTIONS_STATS");
			break;
		case QUERIES:
			return Statistics::get_unified_statistics(pthread_t get_root(), "QUERIES_STATS");
			break;
		case CONFIG:
			return Statistics::get_unified_statistics(pthread_t get_root(), "CONFIG_STATS");
			break;
		default:
			return NULL;
	}
}

pthread_t Statistics::get_root() {
	return root;
}