#ifndef SESSIONSTATS_H
#define SESSIONSTATS_H

#include "SystemStats.h"
#include <time.h>
#include <sys/time.h>

using namespace std;


namespace SystemStatsLib{
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

	class SessionsStats: public SystemStats{

			public:
				SessionsStats();

				void addSessionStats(string key, SessionStats* session);
				SessionStats* getSessionStats(string key);
				void removeSessionStats(string key);

				void addDiskPageReads(int sessionId, int count);
				void addPageReads(int sessionId, int count);
				void addDiskPageWrites(int sessionId, int count);
				void addPageWrites(int sessionId, int count);

				~SessionsStats();
		};
}

#endif
