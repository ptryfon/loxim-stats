/*
 * QueryStats.h
 *
 *  Created on: 19-Nov-09
 *      Author: Bartosz Borkowski
 */

#ifndef QUERYSTATS_H_
#define QUERYSTATS_H_

#include <SystemStats/SystemStats.h>
#include <time.h>
#include <sys/time.h>
#include <list>
#include <map>

using namespace std;

namespace SystemStatsLib {

	class QueryStatistic {
		public:
			Query(uint64_t session_id, std::string text);
			void set_state(unsigned int state);
			
			uint64_t session_id;
			unsigned int disk_IO, weight;
			double milisec;
			std::string text, state;
		private:
			timeval begin;
	};
	
	template<class T>class QueryList<T> {
		public:
			QueryList() : size(1) {}
			void set_size(unsigned int s) {size = s; return;}
			void add_query(T key, QueryStatistic q)
			{
				queries_iterators.push_back(queries.insert(std::pair<T, QueryStatistic>(key, q)));
				if (queries_iterators.size() == size)
				{
					queries.erase(queries_iterators.front());
					queries_iterators.pop_front();
				}
				return;
			}
		private:
			unsigned int size;
			std::multimap<T, QueryStatistic> queries;
			std::list<multimap<T, QueryStatistic>::iterator> queries_iterators;
	};
	
	class AbstractQueriesStats : public AbstractStats {
		public:
			AbstractQueriesStats() : AbstractStats("QUERIES_STATS") {}
			virtual ~AbstactQueriesStats() {}
			
			virtual void add_query(const QueryStatistic&) = 0;
			virtual void begin_execute_query(uint64_t session_id, std::string& stmt) = 0;
			virtual void end_execute_query(uint64_t session_id) = 0;
			virtual end_session(uint64_t session_id) = 0;
			virtual add_disk_io(uint64_t session_id, unsigned int count) = 0;
	};

	class EmptyQueriesStats : public AbstractQueriesStats {
		public:
			void add_query(const QueryStatistic&);
			void begin_execute_query(uint64_t, std::string&) {}
			void end_execute_query(uint64_t) {}
			void end_session(uint64_t) {}
			void add_disk_io(uint64_t, unsigned int) {}
	};
	
	class QueriesStats : public AbstactQueriesStats {
		public:
			QueriesStats();
			
			void add_query(const QueryStatistic& query);
			void begin_execute_query(uint64_t session_id, std::string& stmt);
			void end_execute_query(uint64_t session_id);
			void end_session(uint64_t session_id);
			void add_disk_io(uint64_t session_id, unsigned int count);
			
		private:
			double max_query_time, time_sum, avrg_time;
			unsigned int quieries_amount;
			QueryList<unsigned int> top_IO_queries;
			QueryList<double> top_time_queries;
			map<uint64_t, Query> active_queries;
	};

#endif /* QUERYSTATS_H_ */
