/*
 * QueryStats.h
 *
 *  Created on: 29-lip-08
 *      Author: damianklata
 */

#ifndef QUERYSTATS_H_
#define QUERYSTATS_H_
#include "SystemStats.h"
#include <map>
#include <time.h>
#include <sys/time.h>
#include <list>

using namespace std;

namespace SystemStatsLib {
	/*
	 * How many top queries we want to
	 * collect
	 */
	static const int maxTop = 10;

	/*
	 * Query stats contains information about query that was
	 * execute in specified session:
	 * - last query (text of query)
	 * - state of sessions: active or
	 * 		inactive (that means that transaction whas commited or aborted)
	 * - session id -> session related with this query
	 * - duration time in miliseconds
	 * - sum of disk reads and writes (io)
	 * - weight (optional use in top substatistics) ->
	 *   this is weight of query in some order. For example
	 *   in top time stats the query with the highest time
	 *   of execution will have the lowest weight value.
	 * - key is use only in top sub stats to recognize
	 * specified query. In top stats we can have to queries
	 * from one session and so we cannot identyfy it without
	 * this value.
	 *
	 */
	class QueryStats: public SystemStats {
	protected:
		int diskIO;
		timeval begin;
		string key;
	public:
		QueryStats();
		QueryStats(QueryStats* queryStats);
		~QueryStats();

		void setText(string text);
		string getText();

		void setState(int state);
		string getState();

		void setSessionId(int sessionId);
		int getSessionId();

		void addDiskIO(int count);
		double getMilisec();
		int getDiskIO();

		void setWeight(int weight);

		void setKey(string key);
		string getKey();
	};

	/*
	 * Contains collections of queries statistics
	 * for active or inactive sessions. One query
	 * stats for one sessions. If we execute new
	 * query in session old wild be replaced.
	 */
	class SessionsQueriesStats: public SystemStats {
	public:
		SessionsQueriesStats();
		~SessionsQueriesStats();

		void beginExecuteQuery(int sessionId, const char *stmt);
		void endExecuteQuery(int sessionId);
		void endSession(int sessionId);

		void addDiskIO(int sessionId, int count);

		string createKey(int sessionId);
		QueryStats* getQueryStats(string key);
};

	/*
	 * Statistics of n queries with the
	 * highest sth of execution (only commited
	 * or aborted queries).
	 */
class TopQueriesStats: public SystemStats {
protected:
int qid;
list<QueryStats*> top;
int count;
public:
TopQueriesStats(string name);
virtual ~TopQueriesStats();

string createKey(int id);
virtual bool compare(QueryStats* q1, QueryStats* q2);
void addQuery(QueryStats* queryStats);
};

/*
 * See: TopQueriesStats. Most time consumption queries.
 */
class TopTimeQueriesStats: public TopQueriesStats {
public:
TopTimeQueriesStats();
~TopTimeQueriesStats();

bool compare(QueryStats* q1, QueryStats* q2);
};

/*
 * See: TopQueriesStats. Most io consumption queries.
 */
class TopIOQueriesStats: public TopQueriesStats {
public:
TopIOQueriesStats();
~TopIOQueriesStats();

bool compare(QueryStats* q1, QueryStats* q2);
};

/*
 *	Queries statistics contains 3 substatistics:
 *  - Last queries related with specified session (Session Queries Stats)
 *  - Top time consumption queries
 *  - Top io consumption queries
 *  Also contains information about:
 *  - amout of execution queries
 *  - maximum and average time of executed queries
 */
class QueriesStats: public SystemStats {
protected:
int numberOfQueries;
double sumQueryTime;
double maxQueryTime;
public:
QueriesStats();

void beginExecuteQuery(int sessionId, const char *stmt);
void endExecuteQuery(int sessionId);
void endSession(int sessionId);

void addDiskIO(int sessionId, int count);

int getNumberOfQueries();
SessionsQueriesStats* getSessionsQueriesStats();
TopTimeQueriesStats* getTopTimeQueriesStats();
TopIOQueriesStats* getTopIOQueriesStats();

~QueriesStats();
};

}

#endif /* QUERYSTATS_H_ */
