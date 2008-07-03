#ifndef SESSIONSTATS_H
#define SESSIONSTATS_H

#include "SystemStats.h"

using namespace std;


namespace SystemStatsLib{
	class SessionStats: public SystemStats{

		public:
			SessionStats();

			void setStartTime(string value);
			string getStartTime();

			void setReadsCount(int value);
			int getReadsCount();

			void setWritesCount(int value);
			int getWritesCount();

			void setUserLogin(string value);
			string getUserLogin();

			void setId(int value);
			int getId();
			~SessionStats();
	};

	class SessionsStats: public SystemStats{

			public:
				SessionsStats();

				void addSessionStats(string key, SessionStats* session);
				SessionStats* getSessionStats(string key);
				void removeSessionStats(string key);

				~SessionsStats();
		};
}

#endif
