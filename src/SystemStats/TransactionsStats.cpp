#include <SystemStats/TransactionsStats.h>
#include <sstream>

using namespace SystemStatsLib;

TransactionsStats::TransactionsStats() {
	active_transactions = 0;
	aborted_transactions = 0;
	commited_transactions = 0;

	overall_time = 0.0;
	max_time = 0.0;

	modify_object = 0;
	create_object = 0;
	delete_object = 0;
}

void TransactionsStats::start_transaction(unsigned int tid) {
	++active_transactions;
	transactions_map.insert(pair<unsigned int, time_t>(tid, time(0)));
	return;
}

void TransactionsStats::evaluate_time(unsigned int tid) {
	double transaction_time;
	map<unsigned int, time_t>::iterator it;

	if ((it = transactions_map.find(tid)) == transactions_map.end());
		//throw "Transaction ID not found";

	transaction_time = difftime(time(0), it->second);

	overall_time += transaction_time;

	if (transaction_time > max_time)
		max_time = transaction_time;

	transactions_map.erase(it);

	return;
}

void TransactionsStats::commit_transaction(unsigned int tid) {
	debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "commit\n");
	
	try {
		evaluate_time(tid);
	}
	catch (...) {
	}

	--active_transactions;
	++commited_transactions;
	
	debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "commit ok\n");
}

void TransactionsStats::abort_transaction(unsigned int tid) {
	debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "abort\n");
	try {
	evaluate_time(tid);
	}
	catch (...) {
	}

	--active_transactions;
	++aborted_transactions;
	debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "abort ok\n");
}

double TransactionsStats::get_average_transaction_time() const {
	debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "avg time\n");
	if (!aborted_transactions && !commited_transactions)
		return 0.0;
	else
		return overall_time / (aborted_transactions + commited_transactions);
}

AbstractTransactionsStats & TransactionsStats::operator +=(const TransactionsStats &rhs) {

	active_transactions += rhs.active_transactions;
	aborted_transactions += rhs.aborted_transactions;
	commited_transactions += rhs.commited_transactions;

	modify_object += rhs.modify_object;
	create_object += rhs.create_object;
	delete_object += rhs.delete_object;

	max_time = max(max_time, rhs.max_time);
	overall_time += rhs.overall_time;

	return *this;
}

StatsOutput * TransactionsStats::get_stats_output() const {
	StatsOutput *ret = new StatsOutput;
	
	ret->stats_id = TRANSACTIONS_STATS_ID;

	ret->int_stats.push_back(active_transactions);
	ret->int_names.push_back("active_transactions");
	ret->int_stats.push_back(aborted_transactions);
	ret->int_names.push_back("aborted_transactions");
	ret->int_stats.push_back(commited_transactions);
	ret->int_names.push_back("commited_transactions");
	
	ret->int_stats.push_back(modify_object);
	ret->int_names.push_back("modify_object");
	ret->int_stats.push_back(create_object);
	ret->int_names.push_back("create_object");
	ret->int_stats.push_back(delete_object);
	ret->int_names.push_back("delete_object");

	ret->double_stats.push_back(max_time);
	ret->double_names.push_back("max_time");
	ret->double_stats.push_back(overall_time);
	ret->double_names.push_back("overall_time");
	ret->double_stats.push_back(get_average_transaction_time());
	ret->double_names.push_back("avrg_transaction_time");
	
	return ret;
}
