#include <assert.h>

#include <SystemStats/QueriesStats.h>
#include <SystemStats/Statistics.h>
#include <Config/SBQLConfig.h>

using namespace SystemStatsLib;

QueryInformation::QueryInformation(uint64_t s_id, const std::string& t) :
		session_id(s_id), disk_IO(0), weight(0), milisec(0.0), text(t), state() {
	set_state(1);
}

QueryStatistic::QueryStatistic(uint64_t session_id, const std::string &text) : query_information(session_id, text) {
	Statistics::get_statistics().get_queries_stats().add_query(*this);
}

QueryStatistic::~QueryStatistic() {
	Statistics::get_statistics().get_queries_stats().end_execute_query(get_information().session_id);
}

void QueryInformation::set_state(unsigned int s) {
	if (s == 1)
	{
		disk_IO = 0;
		gettimeofday(&begin, NULL);
		state = "ACTIVE";
	}
	else
	{
		timeval end;
		gettimeofday(&end, NULL);
		
		milisec = (end.tv_sec - begin.tv_sec) * 1000 + (end.tv_usec + begin.tv_usec) / 1000;
		state = "INACTIVE";
	}
	return;
}

QueriesStats::QueriesStats() : max_query_time(0.0), time_sum(0.0), avrg_time(0.0), queries_amount(0) {
	 unsigned int statistics_size = Config::SBQLConfig("statistics").getIntDefault("queryStatsAmount", 10);

	assert(statistics_size);
	
	top_IO_queries.set_size(statistics_size);
	top_time_queries.set_size(statistics_size);
}

void QueriesStats::add_query(const QueryStatistic& query) {
	map<uint64_t, QueryInformation>::iterator k;

	debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "add_begin\n");
	
	if ((k = active_queries.find(query.get_information().session_id)) == active_queries.end())
		active_queries.insert(std::pair<uint64_t, QueryInformation>(query.get_information().session_id, query.get_information()));
	else
	{
		k->second.text = query.get_information().text;
		k->second.set_state(1);
	}
	
	++queries_amount;

	debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "add_end\n");
	return;
}

void QueriesStats::begin_execute_query(uint64_t session_id, std::string& stmt) {
	map<uint64_t, QueryInformation>::iterator k;
	
	debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "begin_begin\n");
	if ((k = active_queries.find(session_id)) == active_queries.end())
		active_queries.insert(std::pair<uint64_t, QueryInformation>(session_id, QueryInformation(session_id, stmt)));
	else
	{
		k->second.text = stmt;
		k->second.set_state(1);
	}
	
	++queries_amount;
	debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "begin_end\n");
	return;
}

void QueriesStats::end_execute_query(uint64_t session_id) {
	map<uint64_t, QueryInformation>::iterator k;
	debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "end_begin\n");
	if ((k = active_queries.find(session_id)) != active_queries.end())
	{
		debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "set_state_begin\n");
		k->second.set_state(0);
		debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "set_state_end\n");
		
		if (max_query_time < k->second.milisec)
			max_query_time = k->second.milisec;
		
		time_sum += k->second.milisec;
		avrg_time = time_sum / queries_amount;
		
		debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "add_top\n");
		top_IO_queries.add_query(k->second.disk_IO, k->second);
		debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "add_time\n");
		top_time_queries.add_query(k->second.milisec, k->second);
		debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "add_ok\n");
	}
	debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "end_end\n");
	return;
}

void QueriesStats::end_session(uint64_t session_id) {
	map<uint64_t, QueryInformation>::iterator k;
	debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "session_end_begin\n");
	if ((k = active_queries.find(session_id)) != active_queries.end())
		active_queries.erase(k);
	debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "session_end_end\n");
	return;
}

void QueriesStats::add_disk_io(uint64_t session_id, unsigned int count) {
	map<uint64_t, QueryInformation>::iterator k;
	debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "disk_io_begin\n");
	if ((k = active_queries.find(session_id)) != active_queries.end())
		k->second.disk_IO += count;
	debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "disk_io_end\n");
	
	return;
}
