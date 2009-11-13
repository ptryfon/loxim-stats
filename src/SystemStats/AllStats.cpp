/*
 * AllStats.cpp
 *
 *  Created on: 12-Nov-09
 *      Author: Bartosz Borkowski
 */

#include <SystemStats/AllStats.h>
#include <Config/SBQLConfig.h>

using namespace SystemStatsLib;

StatisticSingleton::StatisticSingleton()
{
/* 
 * I really, really hope that SBQLConfig works this way.
 * Beacause if not than it's not possible to do it in a
 * nice way. We open the config to see which statistics
 * we want active and which we don't want.
 */

	SBQLConfig config("statistics");
	bool b;

	config.getBool("sessionStats", b);
	if (b)
		active_session_stats = &session_stats;
	else
		active_session_stats = &empty_session_stats;

	config.getBool("configsStats", b);
	if (b)
		active_configs_stats = &configs_stats;
	else
		active_configs_stats = &empty_configs_stats;

	config.getBool("storeStats", b);
	if (b)
		active_store_stats = &store_stats;
	else
		active_store_stats = &empty_store_stats;

	config.getBool("transactionStats", b);
	if (b)
		active_transaction_stats = &transaction_stats;
	else
		active_transaction_stats = &empty_transaction_stats;

	config.getBool("queriesStats", b);
	if (b)
		active_queries_stats = &queries_stats;
	else
		active_queries_stats = &empty_queries_stats;
}

void StatisticSingleton::addDiskPageReads(int sessionId, int /*transactionId*/, int count) {
	active_session_stats->addDiskPageReads(sessionId, count);
	active_store_stats->addDiskPageReads(count);
	active_queries_stats->addDiskIO(sessionId, count);
}

void StatisticSingleton::addPageReads(int sessionId, int /*transactionId*/, int count) {
	active_session_stats->addPageReads(sessionId, count);
	active_store_stats->addPageReads(count);
}

void StatisticSingleton::addDiskPageWrites(int sessionId, int /*transactionId*/, int count) {
	active_session_stats->addDiskPageWrites(sessionId, count);
	active_store_stats->addDiskPageWrites(count);
	active_queries_stats->addDiskIO(sessionId, count);
}

void StatisticSingleton::addPageWrites(int sessionId, int /*transactionId*/, int count) {
	active_session_stats->addPageWrites(sessionId, count);
	active_store_stats->addPageWrites(count);
}
