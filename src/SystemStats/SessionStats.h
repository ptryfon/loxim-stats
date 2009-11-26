#ifndef SESSIONSTATS_H
#define SESSIONSTATS_H

#include <time.h>
#include <sys/time.h>
#include <map>
#include <Util/Types.h>
#include <SystemStats/AbstractStats.h>
#include <SystemStats/DiskUsageStats.h>


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
 
	class AbstractSessionStats : public AbstractStats {
		public:
			AbstractSessionStats():AbstractStats("SessionStats"){}
			
			virtual AbstractDiskUsageStats& getDiskUsageStats() = 0;
			virtual const AbstractDiskUsageStats& getDiskUsageStats() const = 0;
			
			virtual void setDurationInSeconds(double seconds) = 0;

			virtual void setStartTime(time_t value) = 0;
			virtual time_t getStartTime() const = 0;

			/* Statystyki odczytu */
			virtual void addDiskPageReads(int count) = 0;
			virtual int getDiskPageReads() const = 0;

			virtual void addPageReads(int count) = 0;
			virtual int getPageReads() const = 0;

			virtual double getPageReadsHit() const = 0;

			/* Statystyki zapisu */
			virtual void addDiskPageWrites(int count) = 0;
			virtual int getDiskPageWrites() const = 0;

			virtual void addPageWrites(int count) = 0;
			virtual int getPageWrites() const = 0;

			virtual double getPageWritesHit() const = 0;

			virtual void setUserLogin(string value) = 0;
			virtual string getUserLogin() const = 0;

			virtual void setId(SessionID value) = 0;
			virtual SessionID getId() const = 0;

			virtual void refreshStats() = 0;
			
			virtual ~AbstractSessionStats();
	};
 
	class SessionStats: public AbstractSessionStats{
	
		protected:
		  	DiskUsageStats diskUsageStats;
			SessionID id;
			string userLogin;
			time_t startTime;
			double durationInSeconds;
			
		public:
			SessionStats();
			
			AbstractDiskUsageStats& getDiskUsageStats() {return diskUsageStats;}
			const AbstractDiskUsageStats& getDiskUsageStats() const {return diskUsageStats;}

			void setDurationInSeconds(double seconds);

			void setStartTime(time_t value);
			time_t getStartTime() const;

			/* Statystyki odczytu */
			void addDiskPageReads(int count) {getDiskUsageStats().addDiskPageReads(count);}
			int getDiskPageReads() const {return getDiskUsageStats().getDiskPageReads();}

			void addPageReads(int count) {getDiskUsageStats().addPageReads(count);}
			int getPageReads() const {return getDiskUsageStats().getPageReads();}

			double getPageReadsHit() const { return getDiskUsageStats().getPageReadsHit(); }

			/* Statystyki zapisu */
			void addDiskPageWrites(int count) {getDiskUsageStats().addDiskPageWrites(count);}
			int getDiskPageWrites() const { return getDiskUsageStats().getDiskPageWrites();}

			void addPageWrites(int count) {getDiskUsageStats().addPageWrites(count);}
			int getPageWrites() const { return getDiskUsageStats().getPageWrites(); }

			double getPageWritesHit() const { return getDiskUsageStats().getPageWritesHit();}

			void setUserLogin(string value);
			string getUserLogin() const;

			void setId(SessionID value);
			SessionID getId() const;

			void refreshStats();
			~SessionStats();
	};
	
	class EmptySessionStats: public AbstractSessionStats {
	
		protected:
	    		EmptyDiskUsageStats diskUsageStats;
		
		public:
			EmptySessionStats(){}
			
			AbstractDiskUsageStats& getDiskUsageStats() {return diskUsageStats;}
			const AbstractDiskUsageStats& getDiskUsageStats() const {return diskUsageStats;}

			void setDurationInSeconds(double seconds){}

			void setStartTime(time_t value) {};
			time_t getStartTime() const {return time(static_cast<time_t *>(0));}
			/* Statystyki odczytu */
			void addDiskPageReads(int count) {}
			int getDiskPageReads() const {return 0;}

			void addPageReads(int count) {}
			int getPageReads() const {return 0;}

			double getPageReadsHit() const { return 0; }

			/* Statystyki zapisu */
			void addDiskPageWrites(int count) {}
			int getDiskPageWrites() const { return 0;}

			void addPageWrites(int count) {}
			int getPageWrites() const { return 0; }

			double getPageWritesHit() const { return 0;}

			void setUserLogin(string value) {};
			string getUserLogin() const {return "";}

			void setId(SessionID value) {}
			SessionID getId() const {return -1;};

			void refreshStats(){}
			~EmptySessionStats();
	};
	
	/**
	 * AbstractSessionsStats class
	 */
	class AbstractSessionsStats : public AbstractStats {
		public:
			AbstractSessionsStats() : AbstractStats("SESSIONS_STATS") {}
			
			virtual void addSessionStats(SessionID key) = 0;
			virtual AbstractSessionStats& getSessionStats(SessionID key) = 0;
			virtual void removeSessionStats(SessionID key) = 0;

			//To jest chyba do wyrzucenia bo tylko dubluje funkcjonalnosc i tylko powoduje jedno wywolanie funkcji wiecej.
			void addDiskPageReads(SessionID sessionId, int count) {this->getSessionStats(sessionId).addDiskPageReads(count);}
			void addPageReads(SessionID sessionId, int count) {this->getSessionStats(sessionId).addPageReads(count);}
			void addDiskPageWrites(SessionID sessionId, int count) {this->getSessionStats(sessionId).addDiskPageWrites(count);}
			void addPageWrites(SessionID sessionId, int count) {this->getSessionStats(sessionId).addPageWrites(count);}
			
			virtual ~AbstractSessionsStats();
	};

	/*
	 * Sessions statistics contains set of
	 * session statistics
	 */
	 
	class SessionsStats: public AbstractSessionsStats{

		protected:
			map<SessionID, SessionStats> sessionsMap;
		public:
			SessionsStats();

			void addSessionStats(SessionID key);
			SessionStats& getSessionStats(SessionID key);
			void removeSessionStats(SessionID key);

			virtual ~SessionsStats();
	};
	
	class EmptySessionsStats: public AbstractSessionsStats{
		private:
			EmptySessionStats emptySessionStats;
	  	public:
			EmptySessionsStats(){}

			void addSessionStats(SessionID key){}
			EmptySessionStats& getSessionStats(SessionID key) {return emptySessionStats;}
			void removeSessionStats(SessionID key) {}

			virtual ~EmptySessionsStats();
	};
}

#endif
