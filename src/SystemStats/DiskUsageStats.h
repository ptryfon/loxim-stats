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
	class AbstractDiskUsageStats {
	  	public:
			AbstractDiskUsageStats();
			
			/* Disk read statistics */
			virtual void addDiskPageReads(int count) = 0;
			virtual int getDiskPageReads() const = 0;

			virtual void addPageReads(int count) = 0;
			virtual int getPageReads() const = 0;

			virtual double getPageReadsHit() const = 0;

			/* Disk write statistics */
			virtual void addDiskPageWrites(int count) = 0;
			virtual int getDiskPageWrites() const = 0;

			virtual void addPageWrites(int count) = 0;
			virtual int getPageWrites() const = 0;

			virtual double getPageWritesHit() const = 0;

			/* Disk read time functions */
			virtual void addReadTime(int bytes, double milisec) = 0;
			virtual double getMinReadSpeed() const = 0;
			virtual double getMaxReadSpeed() const = 0;
			virtual double getAvgReadSpeed() const = 0;
			
			/* Disk write time functions */
			virtual void addWriteTime(int bytes, double milisec) = 0;
			virtual double getMinWriteSpeed() const = 0;
			virtual double getMaxWriteSpeed() const = 0;
			virtual double getAvgWriteSpeed() const = 0;

			virtual ~AbstractDiskUsageStats();
	};

	class DiskUsageStats: public AbstractDiskUsageStats{
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
			DiskUsageStats();

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

			~DiskUsageStats();
			
		protected:
			void updatePageReadsHit();
			void updatePageWritesHit();
	};
	
	class EmptyDiskUsageStats: public AbstractDiskUsageStats {
	  	public:
	 		EmptyDiskUsageStats() {}

			/* Disk read statistics */
			void addDiskPageReads(int count) {}
			int getDiskPageReads() const {return 0;}

			void addPageReads(int count) {}
			int getPageReads() const  {return 0;}

			double getPageReadsHit() const {return 0;}

			/* Disk write statistics */
			void addDiskPageWrites(int count) {}
			int getDiskPageWrites() const {return 0;}

			void addPageWrites(int count) {}
			int getPageWrites() const {return 0;}

			double getPageWritesHit() const {return 0;}

			/* Disk read time functions */
			void addReadTime(int bytes, double milisec) {}
			double getMinReadSpeed() const  {return 0;}
			double getMaxReadSpeed() const  {return 0;}
			double getAvgReadSpeed() const  {return 0;}
			
			/* Disk write time functions */
			void addWriteTime(int bytes, double milisec) {}
			double getMinWriteSpeed() const {return 0;}
			double getMaxWriteSpeed() const {return 0;}
			double getAvgWriteSpeed() const {return 0;}

			~EmptyDiskUsageStats();
	};

}
#endif /* DISKUSAGESTATS_H_ */
