#ifndef SESSIONSTATS_H
#define SESSIONSTATS_H

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
 
 typedef int SessionID;
 
	class AbstractSessionStats{
	 	private:
			AbstractDiskUsageStats diskUsageStats();
		public:
			AbstractSessionStats();
			
			AbstractDiskUsageStats& getDiskUsageStats() {return diskUsageStats;}
			
			virtual void setDurationInSeconds(unsigned int seconds) = 0;

			virtual void setStartTime(string value) = 0;
			virtual string getStartTime() const = 0;

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

			virtual void setUserLogin(string value) = 0
			virtual string getUserLogin() const = 0;

			virtual void setId(SessionID value) = 0;
			virtual SessionID getId() const = 0;

			virtual void refreshStats() = 0;
			
			virtual ~SessionStats();
	}
 
	class SessionStats: public AbstractSessionStats{
	
		protected:
			SessionID id;
			string userLogin;
			timeval begin;
			
		public:
			SessionStats();

			void setDurationInSeconds(unsigned int seconds);

			void setStartTime(string value);
			string getStartTime() const;

			/* Statystyki odczytu */
			void addDiskPageReads(int count) {getDiskUsageStats().addDiskPageReads(count);}
			int getDiskPageReads() {return getDiskUsageStats().getDiskPageReads();}

			void addPageReads(int count) {getDiskUsageStats().addPageReads(count);}
			int getPageReads() {return getDiskUsageStats().getPageReads();}

			double getPageReadsHit() { return getDiskUsageStats().getPageReadsHit(); }

			/* Statystyki zapisu */
			void addDiskPageWrites(int count) {getDiskUsageStats().addDiskPageWrites();}
			int getDiskPageWrites() { return getDiskUsageStats().getDiskPageWrites();}

			void addPageWrites(int count) {getDiskUsageStats().addPageWrites();}
			int getPageWrites() { return getDiskUsageStats().getPageWrites(); }

			double getPageWritesHit() { return getDiskUsageStats().getPageWritesHit();}

			void setUserLogin(string value);
			string getUserLogin() const;

			void setId(SessionID value);
			SessionID getId() const;

			void refreshStats();
			~SessionStats();
	};
	
	class EmptySessionStats: public AbstractSessionStats {
		public:
			SessionStats();

			void setDurationInSeconds(unsigned int seconds);

			void setStartTime(string value) {};
			string getStartTime() const {return "";}

			/* Statystyki odczytu */
			void addDiskPageReads(int count) {}
			int getDiskPageReads() {return 0;}

			void addPageReads(int count) {}
			int getPageReads() {return 0;}

			double getPageReadsHit() { return 0; }

			/* Statystyki zapisu */
			void addDiskPageWrites(int count) {getDiskUsageStats().addDiskPageWrites();}
			int getDiskPageWrites() { return 0;}

			void addPageWrites(int count) {}
			int getPageWrites() { return 0; }

			double getPageWritesHit() { return 0;}

			void setUserLogin(string value) {};
			string getUserLogin() const {return "";};

			void setId(SessionID value) {};
			SessionID getId() const {return -1};

			void refreshStats(){};
			~SessionStats();
	}
	
	/**
	 * AbstractSessionsStats class
	 */
	class AbstractSessionsStats{
		public:
			AbstractSessionsStats() {}
			
			virtual void addSessionStats(SessionID key) = 0;
			virtual SessionStats& getSessionStats(SessionID key) = 0;
			virtual void removeSessionStats(SessionID key) = 0;

			//To jest chyba do wyrzucenia bo tylko dubluje funkcjonalnosc i tylko powoduje jedno wywolanie funkcji wiecej.
			void addDiskPageReads(SessionID sessionId, int count) {this->getSessionStats(sessionId).addDiskPageReads(count);}
			void addPageReads(SessionID sessionId, int count) {this->getSessionStats(sessionId).addPageReads(count);}
			void addDiskPageWrites(SessionID sessionId, int count) {this->getSessionStats(sessionId).addDiskPageWrites(count);}
			void addPageWrites(SessionID sessionId, int count) {this->getSessionStats(sessionId).addPageWrites(count);}
			
			virtual ~AbstractSessionsStats();
	}

	/*
	 * Sessions statistics contains set of
	 * session statistics
	 */
	 
	class SessionsStats: public AbstractSessionsStats{

		map<SessionID, SessionStats> sessionsMap;
		
		public:
			SessionsStats();

			void addSessionStats(SessionID key);
			SessionStats& getSessionStats(SessionID key);
			void removeSessionStats(SessionID key);

			virtual ~SessionsStats();
	};
	
	class EmptySessionsStats: public AbstractSessionsStats{
	  	public:
			EmptySessionsStats(){}

			void addSessionStats(SessionID key){}
			SessionStats& getSessionStats(SessionID key) {return SessionStats();}
			void removeSessionStats(SessionID key) {}

			virtual ~SessionsStats();
	}
}

#endif
