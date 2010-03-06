/*
 * Statistics.h
 *
 *  Created on: 12-Nov-09
 *      Author: Bartosz Borkowski
 */

#ifndef ALLSTATS_H_
#define ALLSTATS_H_

#include <pthread.h>
#include <string>

#include <SystemStats/SessionStats.h>
#include <SystemStats/ConfigStats.h>
#include <SystemStats/StoreStats.h>
#include <SystemStats/QueriesStats.h>
#include <SystemStats/TransactionsStats.h>
#include <SystemStats/AbstractStats.h>
#include <Errors/ErrorConsole.h>

namespace SystemStatsLib {
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

		/** Stats administration methods */
		
		AbstractSessionsStats & get_sessions_stats() {
			debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "get_session_stats\n");
			return *active_sessions_stats;
		}
		AbstractConfigStats & get_config_stats() {
			debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "get_config_stats\n");
			return *active_config_stats;
		}
		AbstractStoreStats & get_store_stats() {
			debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "get_store_stats\n");
			return *active_store_stats;
		}
		AbstractTransactionsStats & get_transactions_stats() {
			debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "get_transaction_stats\n");
			return *active_transactions_stats;
		}
		AbstractQueriesStats & get_queries_stats() {
			debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "get_queries_stats\n");
			return *active_queries_stats;
		}
		
		AbstractStats & get_abstract_stats(const std::string& name);

		const SessionsStats & get_c_sessions_stats() {
			return sessions_stats;
		}
		const ConfigStats & get_c_config_stats() {
			return config_stats;
		}
		const StoreStats & get_c_store_stats() {
			return store_stats;
		}
		const TransactionsStats & get_c_transactions_stats() {
			return transactions_stats;
		}
		const QueriesStats & get_c_queries_stats() {
			return queries_stats;
		}

		/* not really happy about those 4 methods, but apparently this is the only
		 * way to resolve this without a lot of code
		 */
		void addDiskPageReads(int sessionId, int /*transactionId*/, int count);
		void addPageReads(int sessionId, int /*transactionId*/, int count);
		void addDiskPageWrites(int sessionId, int /*transactionId*/, int count);
		void addPageWrites(int sessionId, int /*transactionId*/, int count);

	private:
		StatisticSingleton(const StatisticSingleton&) {}
		StatisticSingleton& operator =(const StatisticSingleton&) {return *this;}

		AbstractSessionsStats *active_sessions_stats;
		EmptySessionsStats empty_sessions_stats;
		SessionsStats sessions_stats;

		AbstractConfigStats *active_config_stats;
		EmptyConfigStats empty_config_stats;
		ConfigStats config_stats;

		AbstractStoreStats *active_store_stats;
		EmptyStoreStats empty_store_stats;
		StoreStats store_stats;

		AbstractTransactionsStats *active_transactions_stats;
		EmptyTransactionsStats empty_transactions_stats;
		TransactionsStats transactions_stats;

		AbstractQueriesStats *active_queries_stats;
		EmptyQueriesStats empty_queries_stats;
		QueriesStats queries_stats;
	};

	class Statistics {
	/*
	 * This class should be the only way of accessing the StatisticSingleton.
	 */
	public:
		static StatisticSingleton& get_statistics(const pthread_t thread_id);
		static AbstractStats* get_unified_statistics(const pthread_t thread_id,
			const std::string &name);
	private:
		static std::map<pthread_t, StatisticSingleton*> statistic_singletons;
		static pthread_t root;
		static Util::Mutex singleton_mutex;

		Statistics() {}
		Statistics(const Statistics&) {}
		Statistics& operator =(const Statistics&) {return *this;}
	};
}

#endif /* ALLSTATS_H_ */
