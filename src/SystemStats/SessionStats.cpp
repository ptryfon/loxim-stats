#include <SystemStats/SessionStats.h>
#include <sstream>

using namespace SystemStatsLib;

SessionStats::SessionStats(): AbstractSessionStats(), /*SystemStats("SESSION"),*/ diskUsageStats(DiskUsageStats()) {
	setUserLogin("UNAUTHORIZED");

	time_t rawtime;
	time ( &rawtime );
	setStartTime(rawtime);
	
	setDurationInSeconds(0);
}

void SessionStats::refreshStats() {
	time_t act;
	time(&act);

	setDurationInSeconds(difftime(act, startTime));
}

void SessionStats::setDurationInSeconds(double seconds) {
	this->durationInSeconds = seconds;
}

void SessionStats::setStartTime(time_t value) {
	this->startTime = value;
}

time_t SessionStats::getStartTime() const {
	return startTime;
}

void SessionStats::setUserLogin(string value) {
	this->userLogin = value;
}

string SessionStats::getUserLogin() const {
	return userLogin;
}

void SessionStats::setId(SessionID value) {
	this->id = value;
}

SessionID SessionStats::getId() const {
	return id;
}

EmptySessionStats::~EmptySessionStats(){}
AbstractSessionStats::~AbstractSessionStats(){}


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

void SessionsStats::removeSessionStats(SessionID key) {
	sessionsMap.erase(key);
}

SessionsStats::~SessionsStats() {
}

EmptySessionsStats::~EmptySessionsStats(){}
AbstractSessionsStats::~AbstractSessionsStats(){}
