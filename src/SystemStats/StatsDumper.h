/**
 * StatsDumper.h
 * @author: Pawel Tryfon (pt248444)
 * This file is a header file for a class responsible for dumping statistics
 */

#ifndef STATS_DUMPER_H
#define STATS_DUMPER_H

#include <Util/Thread.h>
#include <SystemStats/StatsTypes.h>

namespace SystemStatsLib {
	
	class StatsDumper: public Thread {
		public:
			void start_dumping(StatsScope);
			void stop_dumping(StatsScope);
			virtual void start();
			StatsDumper& get_instance();
			void set_dump_interval(const unsigned int i) { dump_interval = i; }
			
		private:
			std::auto_ptr<StatsDumper> p_instance;
			static Util::Mutex stats_dumper_mutex;
			unsigned int dump_interval; /* Dump interval in miliseconds */
			StatsScope dump_scope;
			
			StatsDumper();
			StatsDumper(const StatsDumper&) {}
			StatsDumper& operator =(const StatsDumper&) {return *this;}
	};
}
 
#endif //STATS_DUMPER_H