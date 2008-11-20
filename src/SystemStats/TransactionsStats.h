/*
 * TransactionStats.h
 *
 *  Created on: 28-lip-08
 *      Author: damianklata
 */

#ifndef TRANSACTIONSTATS_H_
#define TRANSACTIONSTATS_H_

#include "SystemStats.h"
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
	class TransactionsStats: public SystemStats{
		protected:
			int activeTransactions;
			int abortedTransactions;
			int commitedTransactions;

			map<int, timeval> mapOfTransactions;

			double sumOfTimes;
			double maxTime;

			int modifyObject;
			int createObject;
			int deleteObject;
		public:
			TransactionsStats();

			void startTransaction(int tid);
			void commitTransaction(int tid);
			void abortTransaction(int tid);

			void incModifyObject();
			void incCreateObject();
			void incDeleteObject();

			int getActiveTransactions();
			int getCommitedTransactions();
			int getAbortedTransactions();

			double getMaxTransactionTime();
			double getAvgTransactionTime();

			int getModifyObject();
			int getCreateObject();
			int getDeleteObject();

			~TransactionsStats();
	};

}

#endif /* TRANSACTIONSTATS_H_ */
