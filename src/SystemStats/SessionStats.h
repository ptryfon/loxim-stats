#ifndef SESSIONSTATS_H
#define SESSIONSTATS_H

#include <time.h>
#include <sys/time.h>
#include <map>
#include <Util/Types.h>
#include <SystemStats/AbstractStats.h>
#include <SystemStats/DiskUsageStats.h>

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

	class AbstractSessionsStats;
	class SessionsStats;
	class EmptySessionsStats;
	class EmptySessionStats;
	class SessionStats;
	/* Forward declarations.
	 */

	class AbstractSessionStats : public AbstractStats {
		public:
			AbstractSessionStats():AbstractStats("SessionStats"){}
			
			virtual AbstractDiskUsageStats& get_disk_usage_stats() = 0;
			virtual const AbstractDiskUsageStats& get_disk_usage_stats() const = 0;
			
			virtual void set_duration_in_seconds(double seconds) = 0;

			virtual void set_start_time(time_t value) = 0;
			virtual time_t get_start_time() const = 0;

			/* Statystyki odczytu */
			virtual void add_disk_page_reads(int count) = 0;
			virtual int get_disk_page_reads() const = 0;

			virtual void add_page_reads(int count) = 0;
			virtual int get_page_reads() const = 0;

			virtual double get_page_reads_hit() const = 0;

			/* Statystyki zapisu */
			virtual void add_disk_page_writes(int count) = 0;
			virtual int get_disk_page_writes() const = 0;

			virtual void add_page_writes(int count) = 0;
			virtual int get_page_writes() const = 0;

			virtual double get_page_writes_hit() const = 0;

			virtual void set_user_login(std::string value) = 0;
			virtual std::string get_user_login() const = 0;

			virtual void set_id(SessionID value) = 0;
			virtual SessionID get_id() const = 0;

			virtual void refresh_stats() = 0;
			
			AbstractSessionStats & operator +=(const EmptySessionStats &) {return *this;}
			AbstractSessionStats & operator +=(const SessionStats &);
			
			virtual ~AbstractSessionStats();
	};
 
	class SessionStats: public AbstractSessionStats{
	
		protected:
		  	DiskUsageStats disk_usage_stats;
			SessionID id;
			std::string user_login;
			time_t start_time;
			double duration_in_seconds;
			
		public:
			SessionStats();
			
			AbstractDiskUsageStats& get_disk_usage_stats() {return disk_usage_stats;}
			const AbstractDiskUsageStats& get_disk_usage_stats() const {return disk_usage_stats;}

			void set_duration_in_seconds(double seconds);

			void set_start_time(time_t value);
			time_t get_start_time() const;

			/* Statystyki odczytu */
			void add_disk_page_reads(int count) {get_disk_usage_stats().add_disk_page_reads(count);}
			int get_disk_page_reads() const {return get_disk_usage_stats().get_disk_page_reads();}

			void add_page_reads(int count) {get_disk_usage_stats().add_page_reads(count);}
			int get_page_reads() const {return get_disk_usage_stats().get_page_reads();}

			double get_page_reads_hit() const { return get_disk_usage_stats().get_page_reads_hit(); }

			/* Statystyki zapisu */
			void add_disk_page_writes(int count) {get_disk_usage_stats().add_disk_page_writes(count);}
			int get_disk_page_writes() const { return get_disk_usage_stats().get_disk_page_writes();}

			void add_page_writes(int count) {get_disk_usage_stats().add_page_writes(count);}
			int get_page_writes() const { return get_disk_usage_stats().get_page_writes(); }

			double get_page_writes_hit() const { return get_disk_usage_stats().get_page_writes_hit();}

			void set_user_login(std::string value);
			std::string get_user_login() const;

			void set_id(SessionID value);
			SessionID get_id() const;

			void refresh_stats();

			AbstractSessionStats & operator +=(const SessionStats &);
			StatsOutput * get_stats_output() const {EMPTY_STATS_OUTPUT}
			void get_stats_output(StatsOutput*) const;

			~SessionStats();
	};
	
	class EmptySessionStats: public AbstractSessionStats {
	
		protected:
	    		EmptyDiskUsageStats disk_usage_stats;
		
		public:
			EmptySessionStats(){}
			
			AbstractDiskUsageStats& get_disk_usage_stats() {return disk_usage_stats;}
			const AbstractDiskUsageStats& get_disk_usage_stats() const {return disk_usage_stats;}

			void set_duration_in_seconds(double seconds){}

			void set_start_time(time_t value) {};
			time_t get_start_time() const {return time(static_cast<time_t *>(0));}
			/* Statystyki odczytu */
			void add_disk_page_reads(int count) {}
			int get_disk_page_reads() const {return 0;}

			void add_page_reads(int count) {}
			int get_page_reads() const {return 0;}

			double get_page_reads_hit() const { return 0; }

			/* Statystyki zapisu */
			void add_disk_page_writes(int count) {}
			int get_disk_page_writes() const { return 0;}

			void add_page_writes(int count) {}
			int get_page_writes() const { return 0; }

			double get_page_writes_hit() const { return 0;}

			void set_user_login(std::string value) {};
			std::string get_user_login() const {return "";}

			void set_id(SessionID value) {}
			SessionID get_id() const {return -1;};

			void refresh_stats(){}
			~EmptySessionStats();

			AbstractSessionStats & operator +=(const SessionStats &) {return *this;}
			StatsOutput * get_stats_output() const {EMPTY_STATS_OUTPUT}
	};
	
	/**
	 * AbstractSessionStats class
	 */
	class AbstractSessionsStats : public AbstractStats {
		public:
			AbstractSessionsStats() : AbstractStats("SESSIONS_STATS") {}
			
			virtual void add_session_stats(SessionID key) = 0;
			virtual AbstractSessionStats& get_session_stats(SessionID key) = 0;
			virtual void remove_session_stats(SessionID key) = 0;

			//To jest chyba do wyrzucenia bo tylko dubluje funkcjonalnosc i tylko powoduje jedno wywolanie funkcji wiecej.
			void add_disk_page_reads(SessionID session_id, int count) {this->get_session_stats(session_id).add_disk_page_reads(count);}
			void add_page_reads(SessionID session_id, int count) {this->get_session_stats(session_id).add_page_reads(count);}
			void add_disk_page_writes(SessionID session_id, int count) {this->get_session_stats(session_id).add_disk_page_writes(count);}
			void add_page_writes(SessionID session_id, int count) {this->get_session_stats(session_id).add_page_writes(count);}

			virtual AbstractSessionsStats & operator +=(const SessionsStats &rhs) = 0;
			AbstractSessionsStats & operator +=(const EmptySessionsStats &) {return *this;}

			virtual ~AbstractSessionsStats();
	};

	/*
	 * Sessions statistics contains set of
	 * session statistics
	 */

	class SessionsStats: public AbstractSessionsStats{

		protected:
			std::map<SessionID, SessionStats> sessionsMap;
		public:
			SessionsStats();

			void add_session_stats(SessionID key);
			SessionStats& get_session_stats(SessionID key);
			void remove_session_stats(SessionID key);

			AbstractSessionsStats & operator +=(const SessionsStats &);
			StatsOutput * get_stats_output() const;

			~SessionsStats();
	};
	
	class EmptySessionsStats: public AbstractSessionsStats{
		private:
			EmptySessionStats emptySessionStats;
	  	public:
			EmptySessionsStats(){}

			void add_session_stats(SessionID key){}
			EmptySessionStats& get_session_stats(SessionID key) {return emptySessionStats;}
			void remove_session_stats(SessionID key) {}

			AbstractSessionsStats & operator +=(const SessionsStats &) {return *this;}
			StatsOutput * get_stats_output() const {EMPTY_STATS_OUTPUT}

			~EmptySessionsStats();
	};
}

#endif
