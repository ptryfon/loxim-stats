/*
 * StoreStats.h
 *
 *  Created on: 25-lip-08
 *      Author: damianklata
 */

#ifndef STORESTATS_H_
#define STORESTATS_H_

#include <SystemStats/SystemStats.h>

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
	class StoreStats: public SystemStats{
		protected:
			int diskPageReads;
			int pageReads;
			int diskPageWrites;
			int pageWrites;

			double maxspeed;
			double minspeed;
			double allmilisec;
			double allbytes;
		public:
			StoreStats();

			/* Statystyki odczytu */
			void addDiskPageReads(int count);
			int getDiskPageReads();

			void addPageReads(int count);
			int getPageReads();

			double getPageReadsHit();

			/* Statystyki zapisu */
			void addDiskPageWrites(int count);
			int getDiskPageWrites();

			void addPageWrites(int count);
			int getPageWrites();

			double getPageWritesHit();

			/* Czasy odczytów danych */
			void addReadTime(int bytes, double milisec);
			string getMinReadSpeed();
			string getMaxReadSpeed();
			string getAvgReadSpeed();

			~StoreStats();
	};

}
#endif /* STORESTATS_H_ */
