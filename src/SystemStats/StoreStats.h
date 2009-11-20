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

using namespace std;

namespace SystemStatsLib{
/*
 * Store stats collect stats related with store module:
 * - amount of readed pages in memory (or disk)
 * - amount of readed pages from disk
 * - hit ratio - (1 - disk reads / all reads)
 * - the same for write
 * - minimum, maximum and average time of page read
 */
	class AbstractStoreStats : public AbstractStats {
		public:
			AbstractStoreStats() : AbstractStats("STORE_STATS") {}
			virtual AbstractDiskUsageStats& getDiskUsageStats() = 0;
			virtual ~AbstractStoreStats() {}
			void addDiskPageReads(int count) { this->getDiskUsageStats().addDiskPageReads(count); }
			void addPageReads(int count) { this->getDiskUsageStats().addPageReads(count); }
			void addDiskPageWrites(int count) { this->getDiskUsageStats().addDiskPageWrites(count); }
			void addPageWrites(int count) { this->getDiskUsageStats().addPageWrites(count); }
			void addReadTime(int bytes, double milisec) {this->getDiskUsageStats().addReadTime(bytes, milisec);}
	};
	
	class StoreStats: public AbstractStoreStats{
		protected:
			DiskUsageStats diskUsageStats;
		public:
			StoreStats():diskUsageStats(DiskUsageStats()){};
			DiskUsageStats& getDiskUsageStats() {return diskUsageStats;}
	};
	
	class EmptyStoreStats: public AbstractStoreStats{
		protected:
			EmptyDiskUsageStats diskUsageStats;
		public:
			EmptyStoreStats(): diskUsageStats(EmptyDiskUsageStats()){};
			EmptyDiskUsageStats& getDiskUsageStats() {return diskUsageStats;}
	};

}
#endif /* STORESTATS_H_ */
