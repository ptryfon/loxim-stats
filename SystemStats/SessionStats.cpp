#include "SessionStats.h"
#include <time.h>

using namespace SystemStatsLib;

SessionStats::SessionStats(): SystemStats("SESSION") {
	setReadsCount(0);
	setWritesCount(0);
	setUserLogin("UNAUTHORIZED");
	time_t rawtime;

	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	setStartTime(asctime(timeinfo));
}

void SessionStats::setStartTime(string value) {
	setStringStats("START_TIME", value);
}

string SessionStats::getStartTime() {
	return getStringStats("START_TIME");
}

void SessionStats::setReadsCount(int value) {
	setIntStats("READS_COUNT", value);
}

int SessionStats::getReadsCount() {
	return getIntStats("READS_COUNT");
}

void SessionStats::setWritesCount(int value) {
	setIntStats("WRITES_COUNT", value);
}

int SessionStats::getWritesCount() {
	return getIntStats("WRITES_COUNT");
}

void SessionStats::setUserLogin(string value) {
	setStringStats("USER_LOGIN", value);
}

string SessionStats::getUserLogin() {
	return getStringStats("USER_LOGIN");
}
void SessionStats::setId(int value) {
	setIntStats("SESSION_ID", value);
}

int SessionStats::getId() {
	return getIntStats("SESSION_ID");
}


SessionStats::~SessionStats() {
}

SessionsStats::SessionsStats(): SystemStats("SESSIONS") {

}

void SessionsStats::addSessionStats(string key, SessionStats* session) {
	setStatsStats(key, session);
}

SessionStats* SessionsStats::getSessionStats(string key) {
	return dynamic_cast<SessionStats*>(getStatsStats(key));
}

void SessionsStats::removeSessionStats(string key) {
	removeStats(key);
}

SessionsStats::~SessionsStats() {

}
