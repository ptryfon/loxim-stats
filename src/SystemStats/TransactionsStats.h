/*
 * TransactionStats.h
 *
 *  Created on: 12-Nov-09
 *      Author: Bartosz Borkowski
 */

#ifndef TRANSACTIONSTATS_H_
#define TRANSACTIONSTATS_H_

#include <SystemStats/AbstractStats.h>
#include <map>
#include <time.h>
#include <sys/time.h>
#include <Errors/ErrorConsole.h>

using namespace std;

namespace SystemStatsLib{
/*
 * Trasaction statistics collect information about:
 * - modify, create and delete object calls in trasactions
 * - amount of active, aborted and commited trasactions
 * - maximum and average transaction time
 */

	class AbstractTransactionsStats;
	class TransactionsStats;
	class EmptyTransactionsStats;
	/* Forward declarations.
	 */

class AbstractTransactionsStats : public AbstractStats {
	public:
		AbstractTransactionsStats() : AbstractStats("TRANSACTIONS_STATS") {}
		virtual ~AbstractTransactionsStats() {}

		virtual void start_transaction(unsigned int) = 0;
		virtual void commit_transaction(unsigned int) = 0;
		virtual void abort_transaction(unsigned int) = 0;

		virtual unsigned int get_active_transactions() const = 0;
		virtual unsigned int get_commited_transactions() const = 0;
		virtual unsigned int get_aborted_transactions() const = 0;

		virtual double get_average_transaction_time() const = 0;
		virtual double get_max_transaction_time() const = 0;

		virtual void increment_modify_object() = 0;
		virtual void increment_create_object() = 0;
		virtual void increment_delete_object() = 0;

		virtual AbstractTransactionsStats & operator +=(const TransactionsStats &rhs) = 0;
		AbstractTransactionsStats & operator +=(const EmptyTransactionsStats &) {return *this;}
};

	class TransactionsStats : public AbstractTransactionsStats {
	public:
		TransactionsStats();

		void start_transaction(unsigned int tid);
		void commit_transaction(unsigned int tid);
		void abort_transaction(unsigned int tid);

		unsigned int get_active_transactions() const {debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "active\n"); return active_transactions;}
		unsigned int get_commited_transactions() const {debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "commited\n"); return commited_transactions;}
		unsigned int get_aborted_transactions() const {debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "aborted\n"); return aborted_transactions;}

		double get_average_transaction_time() const;
		double get_max_transaction_time() const {debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "maxtime\n"); return max_time;}

		void increment_modify_object() {++modify_object;}
		void increment_create_object() {++create_object;}
		void increment_delete_object() {++delete_object;}

		AbstractTransactionsStats & operator +=(const TransactionsStats &rhs);
		StatsOutput * get_stats_output() const;

	private:
		void evaluate_time(unsigned int tid);

		map<unsigned int, time_t> transactions_map;
		double overall_time;
		double max_time;

		unsigned int active_transactions;
		unsigned int aborted_transactions;
		unsigned int commited_transactions;

		unsigned int modify_object;
		unsigned int create_object;
		unsigned int delete_object;
};

	class EmptyTransactionsStats : public AbstractTransactionsStats {
		public:
			~EmptyTransactionsStats() {}

		void start_transaction(unsigned int) {}
		void commit_transaction(unsigned int) {}
		void abort_transaction(unsigned int) {}

		unsigned int get_active_transactions() const {return 0;}
		unsigned int get_commited_transactions() const {return 0;}
		unsigned int get_aborted_transactions() const {return 0;}

		double get_average_transaction_time() const {return 0.0;}
		double get_max_transaction_time() const {return 0.0;}

		void increment_modify_object() {}
		void increment_create_object() {}
		void increment_delete_object() {}

		AbstractTransactionsStats & operator +=(const TransactionsStats &) {return *this;}
		StatsOutput * get_stats_output() const {EMPTY_STATS_OUTPUT}
};
}

#endif /* TRANSACTIONSTATS_H_ */
