/*
 * Statistics.cpp
 *
 *  Created on: 12-Nov-09
 *      Author: Bartosz Borkowski
 */

#include <SystemStats/Statistics.h>
#include <Config/SBQLConfig.h>

using namespace SystemStatsLib;

StatisticSingleton::StatisticSingleton() {
/* 
 * I really, really hope that SBQLConfig works this way.
 * Beacause if not than it's not possible to do it in a
 * nice way. We open the config to see which statistics
 * we want active and which we don't want.
 */

	//SBQLConfig config("statistics");
	bool b = true;

	//config.getBool("sessionStats", b);
	if (b)
		active_sessions_stats = &sessions_stats;
	else
		active_sessions_stats = &empty_sessions_stats;

	//config.getBool("configsStats", b);
	if (b)
		active_configs_stats = &configs_stats;
	else
		active_configs_stats = &empty_configs_stats;

	//config.getBool("storeStats", b);
	if (b)
		active_store_stats = &store_stats;
	else
		active_store_stats = &empty_store_stats;

	//config.getBool("transactionStats", b);
	if (b)
		active_transactions_stats = &transactions_stats;
	else
		active_transactions_stats = &empty_transactions_stats;

	//config.getBool("queriesStats", b);
	if (b)
		active_queries_stats = &queries_stats;
	else
		active_queries_stats = &empty_queries_stats;
}

AbstractStats& StatisticSingleton::get_abstract_stats(const string& name) {
	if (name == "SESSIONS_STATS")
		return *active_sessions_stats;
	else if (name == "STORE_STATS")
		return *active_store_stats;
	else if (name == "CONFIGS_STATS")
		return *active_configs_stats;
	else if (name == "TRANSACTIONS_STATS")
		return *active_transactions_stats;
	else if (name =="QUERIES_STATS")
		return *active_queries_stats;
	else
		throw ("Unrecognised stats name");
}

void StatisticSingleton::addDiskPageReads(int sessionId, int /*transactionId*/, int count) {
	active_sessions_stats->addDiskPageReads(sessionId, count);
	active_store_stats->addDiskPageReads(count);
	active_queries_stats->add_disk_io(sessionId, count);
}

void StatisticSingleton::addPageReads(int sessionId, int /*transactionId*/, int count) {
	active_sessions_stats->addPageReads(sessionId, count);
	active_store_stats->addPageReads(count);
}

void StatisticSingleton::addDiskPageWrites(int sessionId, int /*transactionId*/, int count) {
	active_sessions_stats->addDiskPageWrites(sessionId, count);
	active_store_stats->addDiskPageWrites(count);
	active_queries_stats->add_disk_io(sessionId, count);
}

void StatisticSingleton::addPageWrites(int sessionId, int /*transactionId*/, int count) {
	active_sessions_stats->addPageWrites(sessionId, count);
	active_store_stats->addPageWrites(count);
}
