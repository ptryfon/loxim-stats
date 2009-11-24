#include <assert.h>

#include <SystemStats/QueriesStats.h>
#include <SystemStats/Statistics.h>
#include <Config/SBQLConfig.h>

using namespace SystemStatsLib;

QueryStatistic::QueryStatistic(uint64_t s_id, const std::string& t) :
		session_id(s_id), disk_IO(0), weight(0), milisec(0.0), text(t), state() {
	set_state(1);
	Statistics::get_statistics().get_queries_stats().add_query(*this);
}

QueryStatistic::~QueryStatistic() {
	Statistics::get_statistics().get_queries_stats().end_execute_query(session_id);
}

void QueryStatistic::set_state(unsigned int s) {
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
	map<uint64_t, QueryStatistic>::iterator k;
	
	if ((k = active_queries.find(query.session_id)) == active_queries.end())
		active_queries.insert(std::pair<uint64_t, QueryStatistic>(query.session_id, query));
	else
	{
		k->second.text = query.text;
		k->second.set_state(1);
	}
	
	++queries_amount;
	
	return;
}

void QueriesStats::begin_execute_query(uint64_t session_id, std::string& stmt) {
	map<uint64_t, QueryStatistic>::iterator k;
	
	if ((k = active_queries.find(session_id)) == active_queries.end())
		active_queries.insert(std::pair<uint64_t, QueryStatistic>(session_id, QueryStatistic(session_id, stmt)));
	else
	{
		k->second.text = stmt;
		k->second.set_state(1);
	}
	
	++queries_amount;
	
	return;
}

void QueriesStats::end_execute_query(uint64_t session_id) {
	map<uint64_t, QueryStatistic>::iterator k;
	
	if ((k = active_queries.find(session_id)) != active_queries.end())
	{
		k->second.set_state(0);
		
		if (max_query_time < k->second.milisec)
			max_query_time = k->second.milisec;
		
		time_sum += k->second.milisec;
		avrg_time = time_sum / queries_amount;
		
		top_IO_queries.add_query(k->second.disk_IO, k->second);
		top_time_queries.add_query(k->second.milisec, k->second);
	}
	
	return;
}

void QueriesStats::end_session(uint64_t session_id) {
	map<uint64_t, QueryStatistic>::iterator k;
	
	if ((k = active_queries.find(session_id)) != active_queries.end())
		active_queries.erase(k);
	
	return;
}

void QueriesStats::add_disk_io(uint64_t session_id, unsigned int count) {
	map<uint64_t, QueryStatistic>::iterator k;
	
	if ((k = active_queries.find(session_id)) != active_queries.end())
		k->second.disk_IO += count;
	
	return;
}
