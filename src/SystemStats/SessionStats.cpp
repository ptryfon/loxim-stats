#include <SystemStats/SessionStats.h>
#include <sstream>

using namespace SystemStatsLib;

SessionStats::SessionStats(): SystemStats("SESSION") {
	setUserLogin("UNAUTHORIZED");
	diskPageReads = 0;
	pageReads = 0;
	diskPageWrites = 0;
	pageWrites = 0;
	addDiskPageReads(0);
	addPageReads(0);
	addDiskPageWrites(0);
	addPageWrites(0);
	gettimeofday(&begin,NULL);

	time_t rawtime;
    struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	setStartTime(asctime(timeinfo));
	setDurationInSeconds(0);
}

void SessionStats::refreshStats() {
	timeval act;
	gettimeofday(&act,NULL);

	setDurationInSeconds(act.tv_sec - begin.tv_sec);
}

void SessionStats::setDurationInSeconds(int seconds) {
	setIntStats("DURATION_IN_SECONDS", seconds);
}

void SessionStats::setStartTime(string value) {
	setStringStats("START_TIME", value);
}

string SessionStats::getStartTime() {
	return getStringStats("START_TIME");
}

void SessionStats::addDiskPageReads(int count) {
	diskPageReads += count;
	setIntStats("DISK_PAGE_READS", diskPageReads);
	double hit = 0.0;
	if (pageReads > 0) {
		hit = 100 * ((1.0 * (pageReads - diskPageReads)) / pageReads);
	}
	setDoubleStats("PAGE_READS_HIT", hit);
}

int SessionStats::getDiskPageReads() {
	return getIntStats("DISK_PAGE_READS");
}

void SessionStats::addPageReads(int count) {
	pageReads += count;
	setIntStats("PAGE_READS", pageReads);
	double hit = 0.0;
	if (pageReads > 0) {
		hit = 100 * ((1.0 * (pageReads - diskPageReads)) / pageReads);
	}
	setDoubleStats("PAGE_READS_HIT", hit);
}

int SessionStats::getPageReads() {
	return getIntStats("PAGE_READS");
}

double SessionStats::getPageReadsHit() {
	return getDoubleStats("PAGE_READS_HIT");
}

void SessionStats::addDiskPageWrites(int count) {
	diskPageWrites += count;
	setIntStats("DISK_PAGE_WRITES", diskPageWrites);
	double hit = 0.0;
	if (pageWrites > 0) {
		hit = 100 * ((1.0 * (pageWrites - diskPageWrites)) / pageWrites);
	}
	setDoubleStats("PAGE_WRITES_HIT", hit);
}

int SessionStats::getDiskPageWrites() {
	return getIntStats("DISK_PAGE_WRITES");
}

void SessionStats::addPageWrites(int count) {
	pageWrites += count;
	setIntStats("PAGE_WRITES", pageWrites);
	double hit = 0.0;
	if (pageReads > 0) {
		hit = 100 * ((1.0 * (pageWrites - diskPageWrites)) / pageWrites);
	}
	setDoubleStats("PAGE_WRITES_HIT", hit);
}

int SessionStats::getPageWrites() {
	return getIntStats("PAGE_WRITES");
}

double SessionStats::getPageWritesHit() {
	return getDoubleStats("PAGE_WRITES_HIT");
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

void SessionsStats::addDiskPageReads(int sessionId, int count) {
	string sessionid;
	stringstream ss;
		ss << "SESSION_" << (sessionId);
		ss >> sessionid;

	SessionStats* session = getSessionStats(sessionid);
	if (session != NULL) {
		session->addDiskPageReads(count);
	}
}

void SessionsStats::addPageReads(int sessionId, int count) {
	string sessionid;
	stringstream ss;
		ss << "SESSION_" << (sessionId);
		ss >> sessionid;

	SessionStats* session = getSessionStats(sessionid);
	if (session != NULL) {
		session->addPageReads(count);
	}
}

void SessionsStats::addDiskPageWrites(int sessionId, int count) {
	string sessionid;
	stringstream ss;
		ss << "SESSION_" << (sessionId);
		ss >> sessionid;

	SessionStats* session = getSessionStats(sessionid);
	if (session != NULL) {
		session->addDiskPageWrites(count);
	}
}

void SessionsStats::addPageWrites(int sessionId, int count) {
	string sessionid;
	stringstream ss;
		ss << "SESSION_" << (sessionId);
		ss >> sessionid;

	SessionStats* session = getSessionStats(sessionid);
	if (session != NULL) {
		session->addPageWrites(count);
	}
}

void SessionsStats::addSessionStats(const string &key, SessionStats* session) {
	setStatsStats(key, session);
}

SessionStats* SessionsStats::getSessionStats(const string &key) {
	return dynamic_cast<SessionStats*>(getStatsStats(key));
}

void SessionsStats::removeSessionStats(const string &key) {
	removeStats(key);
}

SessionsStats::~SessionsStats() {

}
