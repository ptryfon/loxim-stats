/*
 * AllStats.h
 *
 *  Created on: 12-Nov-09
 *      Author: Bartosz Borkowski
 */

#ifndef ALLSTATS_H_
#define ALLSTATS_H_

#include <SystemStats/SystemStats.h>
#include <SystemStats/SessionStats.h>
#include <SystemStats/ConfigStats.h>
#include <SystemStats/StoreStats.h>
#include <SystemStats/QueriesStats.h>
#include <SystemStats/TransactionsStats.h>

using namespace std;

namespace SystemStatsLib{
/*
 * This is class that collect all statistics that
 * we use in database. We can access to this class
 * in static way (getHandle method). Through this
 * class we have access from others modules.
 */
	class StatisticSingleton {
	/*
	 * This class is a singleton holding all of the statistics 'modules'.
	 * It exists to make all the statistic gathering flow through one
	 * point, a situation that makes critical section managing easier.
	 * This class should NOT be constructed in any way. It should be
	 * accesed only via the Statistics class, which is defined below.
	 */
	public:
		StatisticSingleton();

		AbstractSessionStats& get_session_stats() {return *active_session_stats;}
		AbstractConfigsStats& get_configs_stats() {return *active_configs_stats;}
		AbstractStoreStats& get_store_stats() {return *active_store_stats;}
		AbstractTransactionsStats& get_transactions_stats() {return *active_transaction_stats;}
		AbstractQueriesStats& get_queries_stats() {return *active_queries_stats;}

		/* not really happy about those 4 methods, but apparently this is the only
		 * way to resolve this without a lot of code
		 */
		void addDiskPageReads(int sessionId, int /*transactionId*/, int count);
		void addPageReads(int sessionId, int /*transactionId*/, int count);
		void addDiskPageWrites(int sessionId, int /*transactionId*/, int count);
		void addPageWrites(int sessionId, int /*transactionId*/, int count);

	private:
		StatisticSingleton(const StatisticSingleton&) {}
		StatisticSingleton& operator =(const StatisticSingleton&) {}

		AbstractSessionStats *active_session_stats;
		EmptySessionStats empty_session_stats;
		SessionStats session_stats;

		AbstractConfigsStats *abstract_configs_stats;
		EmptyConfigsStats empty_configs_stats;
		ConfigsStats configs_stats;

		AbstractStoreStats *abstract_store_stats;
		EmptyStoreStats empty_store_stats;
		StoreStats store_stats;

		AbstractTransactionsStats *abstract_transaction_stats;
		EmptyTransactionStats empty_transaction_stats;
		TransactionStats transaction_stats;

		AbstractQueriesStats *abstract_queries_stats;
		EmptyQueriesStats empty_queries_stats;
		QueriesStats queries_stats;
	};

	class Statistics {
	/*
	 * This class should be the only way of accessing the StatisticSingleton.
	 */
	public:
		static StatisticSingleton& get_statistics() {return statistic_singleton;}
	private:
		static StatisticSingleton statistic_singleton;

		Statistics() {}
		Statistics(const Statistics&) {}
		Statistics& operator =(const Statistics&) {}
	};
}

#endif /* ALLSTATS_H_ */
