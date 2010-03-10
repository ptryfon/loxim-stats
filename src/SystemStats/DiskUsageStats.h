/*
 * DiskUsageStats.h
 *
 *  Created on: 12-09-2009
 *      Author: paweltryfon
 */

#ifndef DISKUSAGESTATS_H
#define DISKUSAGESTATS_H

#include <Util/smartptr.h>
#include <SystemStats/ShareStat.h>
#include <SystemStats/StandardStat.h>

namespace SystemStatsLib{
/*
 * Disk usage stats collect stats of disk operations:
 * - amount of readed pages from memory or disk
 * - amount of readed pages from disk
 * - hit ratio - (1 - disk reads / all reads)
 * - minimum, maximum and average time of page read
 * - the same for write
 */
	class DiskUsageStats;
	class EmptyDiskUsageStats;

	class AbstractDiskUsageStats {
	  	public:
			AbstractDiskUsageStats(){}
			
			/* Disk read statistics */
			virtual void add_disk_page_reads(int count) = 0;
			virtual int get_disk_page_reads() const = 0;

			virtual void add_page_reads(int count) = 0;
			virtual int get_page_reads() const = 0;

			virtual double get_page_reads_hit() const = 0;

			/* Disk write statistics */
			virtual void add_disk_page_writes(int count) = 0;
			virtual int get_disk_page_writes() const = 0;

			virtual void add_page_writes(int count) = 0;
			virtual int get_page_writes() const = 0;

			virtual double get_page_writes_hit() const = 0;

			/* Disk read time functions */
			virtual void add_read_time(int bytes, double milisec) = 0;
			virtual double get_min_read_speed() const = 0;
			virtual double get_max_read_speed() const = 0;
			virtual double get_avg_read_speed() const = 0;
			
			/* Disk write time functions */
			virtual void add_write_time(int bytes, double milisec) = 0;
			virtual double get_min_write_speed() const = 0;
			virtual double get_max_write_speed() const = 0;
			virtual double get_avg_write_speed() const = 0;
			
			virtual AbstractDiskUsageStats & operator +=(const DiskUsageStats &rhs) = 0;
			AbstractDiskUsageStats & operator +=(const EmptyDiskUsageStats &) {return *this;}

			virtual ~AbstractDiskUsageStats();
	};

	class DiskUsageStats: public AbstractDiskUsageStats{

		protected:

			enum AccessType {
				NO_DISK,
				DISK,
				ACCESSTYPE_SIZE
			};

			StandardStatContainer<double>* active_read_speed_stat;
			StandardStatContainer<double>* active_write_speed_stat;
			StandardStatContainer<double> go_read_speed_stat;
			StandardStatContainer<double> go_write_speed_stat;
			StandardStatOnePeriodContainer<double> op_read_speed_stat;
			StandardStatOnePeriodContainer<double> op_write_speed_stat;
			
			ShareStatContainer<AccessType, int> go_page_reads_stat;
			ShareStatOnePeriodContainer<AccessType, int> op_page_reads_stat;
			ShareStatContainer<AccessType, int> go_page_writes_stat;
			ShareStatOnePeriodContainer<AccessType, int> op_page_writes_stat;
			ShareStatContainer<AccessType, int>* active_page_reads_stat;
			ShareStatContainer<AccessType, int>* active_page_writes_stat;
			
		public:
			DiskUsageStats();

			/* Disk read statistics */
			void add_disk_page_reads(int count) { active_page_reads_stat->add(DISK, count); }
			int get_disk_page_reads() const { return active_page_reads_stat->get_global(DISK); }

			void add_page_reads(int count) { active_page_reads_stat->add(NO_DISK, count); }
			int get_page_reads() const { return active_page_reads_stat->get_global(NO_DISK); }

			double get_page_reads_hit() const { return active_page_reads_stat->get_global_share(NO_DISK); }

			/* Disk write statistics */
			void add_disk_page_writes(int count) { active_page_writes_stat->add(DISK, count); }
			int get_disk_page_writes() const { return active_page_writes_stat->get_global(DISK); }

			void add_page_writes(int count) { active_page_writes_stat->add(NO_DISK, count); }
			int get_page_writes() const {return active_page_writes_stat->get_global(NO_DISK); }

			double get_page_writes_hit() const { return active_page_writes_stat->get_global_share(NO_DISK); }

			/* Disk read time functions */
			void add_read_time(int bytes, double milisec);
			double get_min_read_speed() const;
			double get_max_read_speed() const;
			double get_avg_read_speed() const;
			
			/* Disk write time functions */
			void add_write_time(int bytes, double milisec);
			double get_min_write_speed() const;
			double get_max_write_speed() const;
			double get_avg_write_speed() const;
			
			AbstractDiskUsageStats & operator +=(const DiskUsageStats &) {return *this;}

			~DiskUsageStats();
	};
	
	class EmptyDiskUsageStats: public AbstractDiskUsageStats {
	  	public:
	 		EmptyDiskUsageStats() {}

			/* Disk read statistics */
			void add_disk_page_reads(int count) {}
			int get_disk_page_reads() const {return 0;}

			void add_page_reads(int count) {}
			int get_page_reads() const  {return 0;}

			double get_page_reads_hit() const {return 0;}

			/* Disk write statistics */
			void add_disk_page_writes(int count) {}
			int get_disk_page_writes() const {return 0;}

			void add_page_writes(int count) {}
			int get_page_writes() const {return 0;}

			double get_page_writes_hit() const {return 0;}

			/* Disk read time functions */
			void add_read_time(int bytes, double milisec) {}
			double get_min_read_speed() const  {return 0;}
			double get_max_read_speed() const  {return 0;}
			double get_avg_read_speed() const  {return 0;}
			
			/* Disk write time functions */
			void add_write_time(int bytes, double milisec) {}
			double get_min_write_speed() const {return 0;}
			double get_max_write_speed() const {return 0;}
			double get_avg_write_speed() const {return 0;}
			
			AbstractDiskUsageStats & operator +=(const DiskUsageStats &) {return *this;}

			~EmptyDiskUsageStats();
	};

}
#endif /* DISKUSAGESTATS_H_ */
