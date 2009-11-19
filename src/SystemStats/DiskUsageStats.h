/*
 * DiskUsageStats.h
 *
 *  Created on: 12-09-2009
 *      Author: paweltryfon
 */

#ifndef DISKUSAGESTATS_H_
#define DISKUSAGESTATS_H_

namespace SystemStatsLib{
/*
 * Disk usage stats collect stats of disk operations:
 * - amount of readed pages from memory or disk
 * - amount of readed pages from disk
 * - hit ratio - (1 - disk reads / all reads)
 * - minimum, maximum and average time of page read
 * - the same for write
 */
	class DiskUsageStats{
		protected:
			int 	diskPageReads;
			int 	pageReads;
			double 	pageReadsHit;
			int 	diskPageWrites;
			int 	pageWrites;
			double 	pageWritesHit;

			double 	readMaxspeed;
			double 	readMinspeed;
			double 	readAvgSpeed;
			double 	readAllmilisec;
			double 	readAllbytes;
			
			double 	writeMaxspeed;
			double 	writeMinspeed;
			double 	writeAvgSpeed;
			double 	writeAllmilisec;
			double 	writeAllbytes;
			
		public:
			StoreStats();

			/* Disk read statistics */
			void addDiskPageReads(int count);
			int getDiskPageReads() const;

			void addPageReads(int count);
			int getPageReads() const;

			double getPageReadsHit() const;

			/* Disk write statistics */
			void addDiskPageWrites(int count);
			int getDiskPageWrites() const;

			void addPageWrites(int count);
			int getPageWrites() const;

			double getPageWritesHit() const;

			/* Disk read time functions */
			void addReadTime(int bytes, double milisec);
			double getMinReadSpeed() const;
			double getMaxReadSpeed() const;
			double getAvgReadSpeed() const;
			
			/* Disk write time functions */
			void addWriteTime(int bytes, double milisec);
			double getMinWriteSpeed() const;
			double getMaxWriteSpeed() const;
			double getAvgWriteSpeed() const;

			~StoreStats();
			
		protected:
			void updatePageReadsHit();
			void updatePageWritesHit();
	};

}
#endif /* DISKUSAGESTATS_H_ */
