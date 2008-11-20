#include <SystemStats/QueriesStats.h>
#include <sstream>

using namespace SystemStatsLib;

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
/* QUERY STATS */

QueryStats::QueryStats() :
	SystemStats("QUERY") {
	diskIO = 0;
	setIntStats("DISK_IO", diskIO);
	setDoubleStats("MILISEC", 0);
}

QueryStats::QueryStats(QueryStats* queryStats):
SystemStats("QUERY") {

	setIntStats("DISK_IO", queryStats->getIntStats("DISK_IO"));
	setStringStats("TEXT", queryStats->getStringStats("TEXT"));
	setStringStats("STATE", queryStats->getStringStats("STATE"));
	setIntStats("SESSION_ID", queryStats->getIntStats("SESSION_ID"));
	setDoubleStats("MILISEC", queryStats->getDoubleStats("MILISEC"));
}

QueryStats::~QueryStats() {

}

void QueryStats::setText(string text) {
	setStringStats("TEXT", text);
}

string QueryStats::getText() {
	return getStringStats("TEXT");
}

void QueryStats::setState(int state) {
	if (state == 1) {
		diskIO = 0;
		gettimeofday(&begin,NULL);
		setStringStats("STATE", "ACTIVE");
	} else {
		timeval end;

		gettimeofday(&end,NULL);

		double milisec = (end.tv_sec - begin.tv_sec) * 1000 + (end.tv_usec - begin.tv_usec) / 1000;
		setDoubleStats("MILISEC", milisec);
		setStringStats("STATE", "INACTIVE");
	}
}

string QueryStats::getState() {
	return getStringStats("STATE");
}

void QueryStats::setSessionId(int sessionId) {
	setIntStats("SESSION_ID", sessionId);
}

int QueryStats::getSessionId() {
	return getIntStats("SESSION_ID");
}

void QueryStats::addDiskIO(int count) {
	diskIO += count;
	setIntStats("DISK_IO", diskIO);
}

double QueryStats::getMilisec() {
	return getDoubleStats("MILISEC");
}

int QueryStats::getDiskIO() {
	return getIntStats("DISK_IO");
}

void QueryStats::setWeight(int weight) {
	setIntStats("WEIGHT", weight);
}

void QueryStats::setKey(string key) {
	this->key = key;
}

string QueryStats::getKey() {
	return key;
}

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
/* TOP QUERIES STATS */

TopQueriesStats::TopQueriesStats(string name) :
SystemStats(name) {
	qid = 0;
	count = 0;
}

TopQueriesStats::~TopQueriesStats() {

}
string TopQueriesStats::createKey(int id) {
	string id_key;
	stringstream ss;
	ss << "TOP_ID_" << (id);
	ss >> id_key;
	return id_key;
}

bool TopQueriesStats::compare(QueryStats* q1, QueryStats* q2) {
	return true;
}

void TopQueriesStats::addQuery(QueryStats* queryStats) {
	bool found = false;
	if (top.empty()) {
		string qkey = createKey(qid++);
		queryStats->setKey(qkey);
		queryStats->setWeight(1);
		setStatsStats(qkey, queryStats);
		top.push_front(queryStats);
		count++;
	} else {
		if (compare((*top.begin()), queryStats) && count == maxTop) {
			return;
		}

		for(list<QueryStats*>::iterator iter=top.begin(); iter != top.end(); iter++) {
			if (compare((*iter), queryStats)) {
				string qkey = createKey(qid++);
				queryStats->setKey(qkey);
				setStatsStats(qkey, queryStats);

				top.insert(iter, queryStats);
				if (count == maxTop) {
					removeStats((*top.begin())->getKey());
					top.erase(top.begin());
				} else {
					count++;
				}
				found = true;
				break;
			}
		}

		if (!found) {
			string qkey = createKey(qid++);
			queryStats->setKey(qkey);
			setStatsStats(qkey, queryStats);

			top.push_back(queryStats);
			if (count == maxTop) {
				removeStats((*top.begin())->getKey());
				top.erase(top.begin());
			} else {
				count++;
			}
		}

		/* Enumarate weights */
		int w = top.size();
		for(list<QueryStats*>::iterator iter=top.begin(); iter != top.end(); iter++) {
			(*iter)->setWeight(w--);
		}

	}
}

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
/* TOP TIME QUERIES STATS */

TopTimeQueriesStats::TopTimeQueriesStats() :
	TopQueriesStats("TOP_TIME_QUERIES") {
}

TopTimeQueriesStats::~TopTimeQueriesStats() {
}

bool TopTimeQueriesStats::compare(QueryStats* q1, QueryStats* q2) {
	return q1->getMilisec() >= q2->getMilisec();
}

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
/* TOP IO QUERIES STATS */

TopIOQueriesStats::TopIOQueriesStats() :
	TopQueriesStats("TOP_IO_QUERIES") {
}

TopIOQueriesStats::~TopIOQueriesStats() {
}

bool TopIOQueriesStats::compare(QueryStats* q1, QueryStats* q2) {
	return q1->getDiskIO() >= q2->getDiskIO();
}

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
/* SESSIONS STATS */

SessionsQueriesStats::SessionsQueriesStats() :
	SystemStats("SESSIONS_QUERIES") {

}

SessionsQueriesStats::~SessionsQueriesStats() {

}

QueryStats* SessionsQueriesStats::getQueryStats(string key) {
	return dynamic_cast<QueryStats*> (getStatsStats(key));
}

string SessionsQueriesStats::createKey(int sessionId) {
	string sessionid_key;
	stringstream ss;
	ss << "SESSION_" << (sessionId);
	ss >> sessionid_key;
	return sessionid_key;
}

void SessionsQueriesStats::beginExecuteQuery(int sessionId, const char *stmt) {
	string sessionid_key = createKey(sessionId);

	string text(stmt);

	QueryStats* query = getQueryStats(sessionid_key);
	if (query != NULL) {
		query->setText(text);
		query->setState(1);
	} else {
		QueryStats* query = new QueryStats();
		query->setText(text);
		query->setState(1);
		query->setSessionId(sessionId);
		setStatsStats(sessionid_key, query);
	}
}

void SessionsQueriesStats::endExecuteQuery(int sessionId) {
	string sessionid_key = createKey(sessionId);
	QueryStats* query = getQueryStats(sessionid_key);
	if (query != NULL) {
		query->setState(0);
	}
}

void SessionsQueriesStats::endSession(int sessionId) {
	string sessionid_key = createKey(sessionId);

	QueryStats* query = getQueryStats(sessionid_key);
	if (query != NULL) {
		removeStats(sessionid_key);
	}
}

void SessionsQueriesStats::addDiskIO(int sessionId, int count) {
	string sessionid_key = createKey(sessionId);

	QueryStats* query = getQueryStats(sessionid_key);
	if (query != NULL) {
		query->addDiskIO(count);
	}
}

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
/* QUERIES STATS */

QueriesStats::QueriesStats() :
	SystemStats("QUERIES") {
	numberOfQueries = 0;
	sumQueryTime = 0;
	maxQueryTime = 0;

	setDoubleStats("MAX_QUERY_TIME", maxQueryTime);
	setDoubleStats("AVG_QUERY_TIME", 0);
	setIntStats("NUMBER_OF_QUERIES", numberOfQueries);
	setStatsStats("SESSIONS_STATS", new SessionsQueriesStats());
	setStatsStats("TOP_TIME_STATS", new TopTimeQueriesStats(
					));
					setStatsStats("TOP_IO_STATS", new TopIOQueriesStats(
					));
				}

void QueriesStats::beginExecuteQuery(int sessionId, const char *stmt) {
	numberOfQueries++;
	setIntStats("NUMBER_OF_QUERIES", numberOfQueries);
	getSessionsQueriesStats()->beginExecuteQuery(sessionId, stmt);
}

void QueriesStats::endExecuteQuery(int sessionId) {
	getSessionsQueriesStats()->endExecuteQuery(sessionId);
	string sessionid_key = getSessionsQueriesStats()->createKey(sessionId);
	QueryStats* qs = getSessionsQueriesStats()->getQueryStats(sessionid_key);
	if (qs != NULL) {
		double milisec = qs->getMilisec();
		if (milisec> maxQueryTime) {
			maxQueryTime = milisec;
			setDoubleStats("MAX_QUERY_TIME", maxQueryTime);
		}
		sumQueryTime += milisec;
		if (numberOfQueries> 0) {
			double avg = sumQueryTime / numberOfQueries;
			setDoubleStats("AVG_QUERY_TIME", avg);
		}
		getTopTimeQueriesStats()->addQuery(new QueryStats(qs));
		getTopIOQueriesStats()->addQuery(new QueryStats(qs));
	}
}

void QueriesStats::endSession(int sessionId) {
	getSessionsQueriesStats()->endSession(sessionId);
}

void QueriesStats::addDiskIO(int sessionId, int count) {
	getSessionsQueriesStats()->addDiskIO(sessionId, count);
}

int QueriesStats::getNumberOfQueries() {
	return getIntStats("NUMBER_OF_QUERIES");
}

SessionsQueriesStats* QueriesStats::getSessionsQueriesStats() {
	return dynamic_cast<SessionsQueriesStats*> (getStatsStats("SESSIONS_STATS"));
}

TopTimeQueriesStats* QueriesStats::getTopTimeQueriesStats() {
	return dynamic_cast<TopTimeQueriesStats*> (getStatsStats("TOP_TIME_STATS"));
}

TopIOQueriesStats* QueriesStats::getTopIOQueriesStats() {
	return dynamic_cast<TopIOQueriesStats*> (getStatsStats("TOP_IO_STATS"));
}

QueriesStats::~QueriesStats() {
}

