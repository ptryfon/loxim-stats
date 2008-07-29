#include "TransactionsStats.h"
#include <sstream>

using namespace SystemStatsLib;

TransactionsStats::TransactionsStats() :
	SystemStats("TRANSACTIONS") {
	activeTransactions = 0;
	abortedTransactions = 0;
	commitedTransactions = 0;

	sumOfTimes = 0;
	maxTime = 0;

	modifyObject = 0;
	createObject = 0;
	deleteObject = 0;

	setIntStats("ACTIVE_TRANSACTIONS", activeTransactions);
	setIntStats("COMMITED_TRANSACTIONS", commitedTransactions);
	setIntStats("ABORTED_TRANSACTIONS", abortedTransactions);

	setDoubleStats("MAX_TRANSACTION_TIME_IN_SEC", 0);
	setDoubleStats("AVG_TRANSACTION_TIME_IN_SEC", 0);

	setIntStats("MODIFY_OBJECT", modifyObject);
	setIntStats("DELETE_OBJECT", deleteObject);
	setIntStats("CREATE_OBJECT", createObject);

}

void TransactionsStats::startTransaction(int tid) {
	activeTransactions++;
	timeval begin;
	gettimeofday(&begin,NULL);
	mapOfTransactions[tid] = begin;
	setIntStats("ACTIVE_TRANSACTIONS", activeTransactions);
}

void TransactionsStats::abortTransaction(int tid) {
	activeTransactions--;
	abortedTransactions++;


	map<int,timeval>::iterator iter = mapOfTransactions.find(tid);
	  if( iter != mapOfTransactions.end() ) {
			timeval begin, end;

			begin = mapOfTransactions[tid];
			gettimeofday(&end,NULL);
			mapOfTransactions[tid] = end;

			double sec = end.tv_sec - begin.tv_sec;
			if (sec > maxTime) {
				maxTime = sec;
				setDoubleStats("MAX_TRANSACTION_TIME_IN_SEC", maxTime);
			}
			sumOfTimes += sec;
			setDoubleStats("AVG_TRANSACTION_TIME_IN_SEC", (sumOfTimes / (commitedTransactions + abortedTransactions)));
	  }

	setIntStats("ACTIVE_TRANSACTIONS", activeTransactions);
	setIntStats("ABORTED_TRANSACTIONS", abortedTransactions);
}

void TransactionsStats::commitTransaction(int tid) {
	activeTransactions--;
	commitedTransactions++;

	map<int,timeval>::iterator iter = mapOfTransactions.find(tid);
	  if( iter != mapOfTransactions.end() ) {
			timeval begin, end;

			begin = mapOfTransactions[tid];
			gettimeofday(&end,NULL);
			mapOfTransactions[tid] = end;

			double sec = end.tv_sec - begin.tv_sec;
			if (sec > maxTime) {
				maxTime = sec;
				setDoubleStats("MAX_TRANSACTION_TIME_IN_SEC", maxTime);
			}
			sumOfTimes += sec;
			setDoubleStats("AVG_TRANSACTION_TIME_IN_SEC", (sumOfTimes / (commitedTransactions + abortedTransactions)));
	  }

	setIntStats("ACTIVE_TRANSACTIONS", activeTransactions);
	setIntStats("COMMITED_TRANSACTIONS", commitedTransactions);
}

int TransactionsStats::getActiveTransactions() {
	return getIntStats("ACTIVE_TRANSACTIONS");
}

int TransactionsStats::getCommitedTransactions() {
	return getIntStats("COMMITED_TRANSACTIONS");
}

int TransactionsStats::getAbortedTransactions() {
	return getIntStats("ABORTED_TRANSACTIONS");
}

double TransactionsStats::getMaxTransactionTime() {
	return getDoubleStats("MAX_TRANSACTION_TIME_IN_SEC");
}

double TransactionsStats::getAvgTransactionTime() {
	return getDoubleStats("AVG_TRANSACTION_TIME_IN_SEC");
}

void TransactionsStats::incModifyObject() {
	modifyObject++;
	setIntStats("MODIFY_OBJECT", modifyObject);
}

void TransactionsStats::incCreateObject() {
	createObject++;
	setIntStats("CREATE_OBJECT", createObject);
}

void TransactionsStats::incDeleteObject() {
	deleteObject++;
	setIntStats("DELETE_OBJECT", deleteObject);
}

int TransactionsStats::getModifyObject() {
	return getIntStats("MODIFY_OBJECT");
}

int TransactionsStats::getCreateObject() {
	return getIntStats("CREATE_OBJECT");
}

int TransactionsStats::getDeleteObject() {
	return getIntStats("DELETE_OBJECT");
}

TransactionsStats::~TransactionsStats() {
}

