#include <SystemStats/TransactionsStats.h>
#include <sstream>

using namespace SystemStatsLib;

TransactionStats::TransactionStats()
{
	active_transactions = 0;
	aborted_transactions = 0;
	commited_transactions = 0;

	overall_time = 0.0;
	max_time = 0.0;

	modify_object = 0;
	create_object = 0;
	delete_object = 0;
}

void TransactionStats::start_transaction(unsigned int tid)
{
	++active_transactions;
	transactions_map.insert(pair<unsigned int, time_t>(tid, time(0)));
	return;
}

void TransactionStats::evaluate_time(unsigned int tid)
{
	double transaction_time;
	map<unsigned int, time_t>::iterator it;

	if ((it = transactions_map.find(tid)) == transactions_map.end())
		throw "Transaction ID not found";

	transaction_time = difftime(time(0), it->second);

	overall_time += transaction_time;

	if (transaction_time > max_time)
		max_time = transaction_time;

	transactions_map.erase(it);

	return;
}

void TransactionStats::commit_transaction(unsigned int tid)
{
	try {
		evaluate_time(tid);
	}
	catch (...) {
	}

	--active_transactions;
	++commited_transactions;
}

void TransactionStats::abort_transaction(unsigned int tid)
{
	evaluate_time(tid);

	--active_transactions;
	++aborted_transactions;
}

double TransactionsStats::get_average_transaction_time()
{
	if (!aborted_transactions && !commited_transactions)
		return 0.0;
	else
		return overall_time / (aborted_transactions + commited_transactions);
}
