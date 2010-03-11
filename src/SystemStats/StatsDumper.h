/**
 * StatsDumper.h
 * @author: Pawel Tryfon (pt248444)
 * This file is a header file for a class responsible for dumping statistics
 */

#ifndef STATS_DUMPER_H
#define STATS_DUMPER_H

#include <Util/smartptr.h>
#include <Util/Thread.h>
#include <Util/Concurrency.h>
#include <SystemStats/StatsTypes.h>
#include <SystemStats/AbstractStats.h>
#include <time.h>

namespace SystemStatsLib {
	
	class StatsDumper: public Util::Thread {
		public:
			void start_dumping(StatsScope);
			void stop_dumping(StatsScope);
			virtual void start();
			static StatsDumper* get_instance();
			void set_dump_interval(const unsigned int i) { dump_interval = i; }
			void kill() { stop = true; }
			
		private:
			static StatsDumper* p_instance;
			bool stop;
			unsigned int dump_interval; /* Dump interval in miliseconds */
			StatsScope dump_scope;
			time_t start_time;
			std::string dump_dir;
			std::string store_dump_dir;
			std::string sessions_dump_dir;
			std::string transactions_dump_dir;
			
			StatsDumper();
			StatsDumper(const StatsDumper&) {}
			StatsDumper& operator =(const StatsDumper&) {return *this;}
			void dump(std::string dir, StatsOutput* output, time_t time_stamp);
	};
}
 
#endif //STATS_DUMPER_H
