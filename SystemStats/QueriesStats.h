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
	static const int maxTop = 10;

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

class TopTimeQueriesStats: public TopQueriesStats {
public:
TopTimeQueriesStats();
~TopTimeQueriesStats();

bool compare(QueryStats* q1, QueryStats* q2);
};

class TopIOQueriesStats: public TopQueriesStats {
public:
TopIOQueriesStats();
~TopIOQueriesStats();

bool compare(QueryStats* q1, QueryStats* q2);
};

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
