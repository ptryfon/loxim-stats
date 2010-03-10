/*
 * StoreStats.h
 *
 *  Created on: 12-nov-2009
 *      Author: paweltryfon
 */

#ifndef STORESTATS_H_
#define STORESTATS_H_

#include <SystemStats/AbstractStats.h>
#include <SystemStats/DiskUsageStats.h>

namespace SystemStatsLib{
/*
 * Store stats collect stats related with store module:
 * - amount of readed pages in memory (or disk)
 * - amount of readed pages from disk
 * - hit ratio - (1 - disk reads / all reads)
 * - the same for write
 * - minimum, maximum and average time of page read
 */

	class AbstractStoreStats;
	class StoreStats;
	class EmptyStoreStats;
	/* Forward declarations.
	 */

	class AbstractStoreStats : public AbstractStats {
		public:
			AbstractStoreStats() : AbstractStats("STORE_STATS") {}
			virtual AbstractDiskUsageStats& get_disk_usage_stats() = 0;
			virtual ~AbstractStoreStats() {}
			void add_disk_page_reads(int count) { this->get_disk_usage_stats().add_disk_page_reads(count); }
			void add_page_reads(int count) { this->get_disk_usage_stats().add_page_reads(count); }
			void add_disk_page_writes(int count) { this->get_disk_usage_stats().add_disk_page_writes(count); }
			void add_page_writes(int count) { this->get_disk_usage_stats().add_page_writes(count); }
			void add_read_time(int bytes, double milisec) {this->get_disk_usage_stats().add_read_time(bytes, milisec);}

			virtual AbstractStoreStats & operator +=(const StoreStats &rhs) = 0;
			AbstractStoreStats & operator +=(const EmptyStoreStats &) {return *this;}
	};
	
	class StoreStats: public AbstractStoreStats{
		protected:
			DiskUsageStats disk_usage_stats;
		public:
			StoreStats():disk_usage_stats(){};
			DiskUsageStats& get_disk_usage_stats() {return disk_usage_stats;}

			AbstractStoreStats & operator +=(const StoreStats &) {return *this;} //TODO: change to ths nice
			StatsOutput * get_stats_output() const {EMPTY_STATS_OUTPUT} // TODO: this too
	};
	
	class EmptyStoreStats: public AbstractStoreStats{
		protected:
			EmptyDiskUsageStats disk_usage_stats;
		public:
			EmptyStoreStats(): disk_usage_stats(EmptyDiskUsageStats()){};
			EmptyDiskUsageStats& get_disk_usage_stats() {return disk_usage_stats;}

			AbstractStoreStats & operator +=(const StoreStats &) {return *this;}
			StatsOutput * get_stats_output() const {EMPTY_STATS_OUTPUT}
	};

}
#endif /* STORESTATS_H_ */
