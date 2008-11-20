/*
 * AllStats.cpp
 *
 *  Created on: 2-lip-08
 *      Author: damianklata
 */

#include <SystemStats/AllStats.h>

using namespace SystemStatsLib;

AllStats::AllStats(): SystemStats("ALL_STATS") {
	setStatsStats("SESSIONS_STATS", new SessionsStats());
	setStatsStats("STORE_STATS", new StoreStats());
	setStatsStats("CONFIGS_STATS", new ConfigsStats());
	setStatsStats("TRANSACTIONS_STATS", new TransactionsStats());
	setStatsStats("QUERIES_STATS", new QueriesStats());
}

AllStats* AllStats::allStats = NULL;//new TransactionManager();

AllStats* AllStats::getHandle() {
	if (allStats == NULL) {
		allStats = new AllStats();
	}
	return allStats;
}

SessionsStats* AllStats::getSessionsStats() {
	return dynamic_cast<SessionsStats*>(getStatsStats("SESSIONS_STATS"));
}

ConfigsStats* AllStats::getConfigsStats() {
	return dynamic_cast<ConfigsStats*>(getStatsStats("CONFIGS_STATS"));
}

StoreStats* AllStats::getStoreStats() {
	return dynamic_cast<StoreStats*>(getStatsStats("STORE_STATS"));
}

TransactionsStats* AllStats::getTransactionsStats() {
	return dynamic_cast<TransactionsStats*>(getStatsStats("TRANSACTIONS_STATS"));
}

QueriesStats* AllStats::getQueriesStats() {
	return dynamic_cast<QueriesStats*>(getStatsStats("QUERIES_STATS"));
}

void AllStats::addDiskPageReads(int sessionId, int transactionId, int count) {
	getSessionsStats()->addDiskPageReads(sessionId, count);
	getStoreStats()->addDiskPageReads(count);
	getQueriesStats()->addDiskIO(sessionId, count);
}

void AllStats::addPageReads(int sessionId, int transactionId, int count) {
	getSessionsStats()->addPageReads(sessionId, count);
	getStoreStats()->addPageReads(count);
}

void AllStats::addDiskPageWrites(int sessionId, int transactionId, int count) {
	getSessionsStats()->addDiskPageWrites(sessionId, count);
	getStoreStats()->addDiskPageWrites(count);
	getQueriesStats()->addDiskIO(sessionId, count);
}

void AllStats::addPageWrites(int sessionId, int transactionId, int count) {
	getSessionsStats()->addPageWrites(sessionId, count);
	getStoreStats()->addPageWrites(count);
}

AllStats::~AllStats() {
}
