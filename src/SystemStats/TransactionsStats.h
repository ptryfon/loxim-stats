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

using namespace std;

namespace SystemStatsLib{
/*
 * Trasaction statistics collect information about:
 * - modify, create and delete object calls in trasactions
 * - amount of active, aborted and commited trasactions
 * - maximum and average transaction time
 */
class AbstractTransactionStats : public AbstractStats {
	public:
		AbstractTransactionStats() : AbstractStats("TRANSACTIONS_STATS") {}
		virtual ~AbstractTransactionStats();
		virtual void start_transaction(unsigned int) = 0;
		virtual void commit_transaction(unsigned int) = 0;
		virtual void abort_transaction(unsigned int) = 0;

		virtual unsigned int get_active_transactions() = 0;
		virtual unsigned int get_commited_transactions() = 0;
		virtual unsigned int get_aborted_transactions() = 0;

		virtual double get_average_transaction_time() = 0;
		virtual double get_max_transaction_time() = 0;

		virtual void increment_modify_object() = 0;
		virtual void increment_create_object() = 0;
		virtual void increment_delete_object() = 0;
};

	class TransactionsStats : public AbstractTransactionStats {
	public:
		TransactionsStats();

		void start_transaction(unsigned int tid);
		void commit_transaction(unsigned int tid);
		void abort_transaction(unsigned int tid);

		unsigned int get_active_transactions() {return active_transactions;}
		unsigned int get_commited_transactions() {return commited_transactions;}
		unsigned int get_aborted_transactions() {return aborted_transactions;}

		double get_average_transaction_time();
		double get_max_transaction_time() {return max_time;}

		void increment_modify_object() {++modify_object;}
		void increment_create_object() {++create_object;}
		void increment_delete_object() {++delete_object;}

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

	class EmptyTransactionStats : public AbstractTransactionStats {
		public:

		void start_transaction(unsigned int) {}
		void commit_transaction(unsigned int) {}
		void abort_transaction(unsigned int) {}

		unsigned int get_active_transactions() {return 0;}
		unsigned int get_commited_transactions() {return 0;}
		unsigned int get_aborted_transactions() {return 0;}

		double get_average_transaction_time() {return 0.0;}
		double get_max_transaction_time() {return 0.0;}

		void increment_modify_object() {}
		void increment_create_object() {}
		void increment_delete_object() {}
};
}

#endif /* TRANSACTIONSTATS_H_ */
