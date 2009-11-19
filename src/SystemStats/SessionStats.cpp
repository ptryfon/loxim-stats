#include <SystemStats/SessionStats.h>
#include <sstream>

using namespace SystemStatsLib;

SessionStats::SessionStats(): SystemStats("SESSION"), diskUsageStats(DiskUsageStats()) {
	setUserLogin("UNAUTHORIZED");

	gettimeofday(&begin,NULL);

	time_t rawtime;
	time ( &rawtime );
	setStartTime(rawtime);
	
	setDurationInSeconds(0);
}

void SessionStats::refreshStats() {
	timeval act;
	gettimeofday(&act,NULL);

	setDurationInSeconds(act.tv_sec - begin.tv_sec);
}

void SessionStats::setDurationInSeconds(int seconds) {
	this->durationInSeconds = seconds;
}

void SessionStats::setStartTime(time_t value) {
	this->startTime = value;
}

time_t SessionStats::getStartTime() {
	return startTime;
}

void SessionStats::setUserLogin(string value) {
	this->userLogin = value;
}

string SessionStats::getUserLogin() {
	return userLogin;
}

void SessionStats::setId(SessionID value) {
	this->id = value;
}

SessionID SessionStats::getId() {
	return id;
}


SessionStats::~SessionStats() {
}

SessionsStats::SessionsStats() {
}

void SessionsStats::addSessionStats(SessionID key) {
	sessionsMap[key];
}

SessionStats& SessionsStats::getSessionStats(SessionID key) {
	return sessionsMap[key];
}

void SessionsStats::removeSessionStats(const SessionID &key) {
	sessionsMap.erase(key);
}

SessionsStats::~SessionsStats() {

}
