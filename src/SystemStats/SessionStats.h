#ifndef SESSIONSTATS_H
#define SESSIONSTATS_H

#include <SystemStats/SystemStats.h>
#include <time.h>
#include <sys/time.h>

using namespace std;


namespace SystemStatsLib{
/*
 * Session stats contains information about:
 * - amount of readed pages in memory (or disk)
 * - amount of readed pages from disk
 * - hit ratio - (1 - disk reads / all reads)
 * - the same for write
 * - logged user
 * - start session time
 * - duration time in seconds
 */
	class SessionStats: public SystemStats{
		protected:
			int diskPageReads;
			int pageReads;
			int diskPageWrites;
			int pageWrites;
			timeval begin;
		public:
			SessionStats();

			void setDurationInSeconds(int seconds);

			void setStartTime(string value);
			string getStartTime();

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

			void setUserLogin(string value);
			string getUserLogin();

			void setId(int value);
			int getId();

			void refreshStats();
			~SessionStats();
	};

	/*
	 * Sessions statistics contains set of
	 * session statistics
	 */
	class SessionsStats: public SystemStats{

			public:
				SessionsStats();

				void addSessionStats(const string &key, SessionStats* session);
				SessionStats* getSessionStats(const string &key);
				void removeSessionStats(const string &key);

				void addDiskPageReads(int sessionId, int count);
				void addPageReads(int sessionId, int count);
				void addDiskPageWrites(int sessionId, int count);
				void addPageWrites(int sessionId, int count);

				~SessionsStats();
		};
}

#endif
