/*
 * QueryStats.h
 *
 *  Created on: 19-Nov-09
 *      Author: Bartosz Borkowski
 */

#ifndef QUERYSTATS_H_
#define QUERYSTATS_H_

#include <SystemStats/AbstractStats.h>
#include <time.h>
#include <sys/time.h>
#include <list>
#include <map>
#include <stdint.h>
#include <Errors/ErrorConsole.h>
#include <Util/Misc.h>

namespace SystemStatsLib {

	class QueryInformation {
		public:
			QueryInformation(uint64_t session_id, const std::string &text);
			void set_state(unsigned int state);
			
			uint64_t session_id;
			unsigned int disk_IO, weight;
			double milisec;
			std::string text, state;
		private:
			timeval begin;
	};
	
	class QueryStatistic : private Util::NonCopyable {
		public:
			QueryStatistic(uint64_t session_id, const std::string &text);
			~QueryStatistic();
			
			void set_state(unsigned int state) {return query_information.set_state(state);}
			const QueryInformation & get_information() const {return query_information;}

		private:
			QueryInformation query_information;
	};
	
	template<class T>class QueryList {
		public:
			QueryList() : size(1) {}
			void set_size(unsigned int s) {size = s; return;}
			void add_query(T key, const QueryInformation &q)
			{
				debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "inserting\n");
				queries_iterators.push_back(queries.insert(std::pair<T, QueryInformation>(key, q)));
				debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "inserted\n");
				if (queries_iterators.size() == size)
				{
					queries.erase(queries_iterators.front());
					queries_iterators.pop_front();
				}
				debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "if_end\n");
				return;
			}
		private:
			unsigned int size;
			std::multimap<T, QueryInformation> queries;
			std::list<typename std::multimap<T, QueryInformation>::iterator> queries_iterators;
	};
	
	class AbstractQueriesStats : public AbstractStats {
		public:
			AbstractQueriesStats() : AbstractStats("QUERIES_STATS") {}
			virtual ~AbstractQueriesStats() {}
			
			virtual void add_query(const QueryStatistic&) = 0;
			virtual void begin_execute_query(uint64_t session_id, std::string& stmt) = 0;
			virtual void end_execute_query(uint64_t session_id) = 0;
			virtual void end_session(uint64_t session_id) = 0;
			virtual void add_disk_io(uint64_t session_id, unsigned int count) = 0;
	};

	class EmptyQueriesStats : public AbstractQueriesStats {
		public:
			~EmptyQueriesStats() {}

			void add_query(const QueryStatistic&) {}
			void begin_execute_query(uint64_t, std::string&) {}
			void end_execute_query(uint64_t) {}
			void end_session(uint64_t) {}
			void add_disk_io(uint64_t, unsigned int) {}
	};
	
	class QueriesStats : public AbstractQueriesStats {
		public:
			QueriesStats();
			
			void add_query(const QueryStatistic& query);
			void begin_execute_query(uint64_t session_id, std::string& stmt);
			void end_execute_query(uint64_t session_id);
			void end_session(uint64_t session_id);
			void add_disk_io(uint64_t session_id, unsigned int count);
			
		private:
			double max_query_time, time_sum, avrg_time;
			unsigned int queries_amount;
			QueryList<unsigned int> top_IO_queries;
			QueryList<double> top_time_queries;
			std::map<uint64_t, QueryInformation> active_queries;
	};
}

#endif /* QUERYSTATS_H_ */
