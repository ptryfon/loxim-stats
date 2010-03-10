#include <SystemStats/SessionStats.h>
#include <sstream>

using namespace SystemStatsLib;
using namespace std;

SessionStats::SessionStats(): AbstractSessionStats(), /*SystemStats("SESSION"),*/ disk_usage_stats() {
	set_user_login("UNAUTHORIZED");

	time_t rawtime;
	time ( &rawtime );
	set_start_time(rawtime);
	
	set_duration_in_seconds(0);
}

void SessionStats::refresh_stats() {
	time_t act;
	time(&act);

	set_duration_in_seconds(difftime(act, start_time));
}

void SessionStats::set_duration_in_seconds(double seconds) {
	this->duration_in_seconds = seconds;
}

void SessionStats::set_start_time(time_t value) {
	this->start_time = value;
}

time_t SessionStats::get_start_time() const {
	return start_time;
}

void SessionStats::set_user_login(string value) {
	this->user_login = value;
}

string SessionStats::get_user_login() const {
	return user_login;
}

void SessionStats::set_id(SessionID value) {
	this->id = value;
}

SessionID SessionStats::get_id() const {
	return id;
}

AbstractSessionStats & 
SessionStats::operator +=(const SessionStats &rhs)
{
	disk_usage_stats += rhs.disk_usage_stats;
	start_time = std::min(start_time, rhs.start_time);
	duration_in_seconds = std::max(duration_in_seconds, rhs.duration_in_seconds);
	
	return *this;
}

EmptySessionStats::~EmptySessionStats(){}
AbstractSessionStats::~AbstractSessionStats(){}


SessionStats::~SessionStats() {
}

SessionsStats::SessionsStats() {
}

void SessionsStats::add_session_stats(SessionID key) {
	sessionsMap[key];
}

SessionStats& SessionsStats::get_session_stats(SessionID key) {
	return sessionsMap[key];
}

void SessionsStats::remove_session_stats(SessionID key) {
	sessionsMap.erase(key);
}

SessionsStats::~SessionsStats() {
}

AbstractSessionsStats & 
SessionsStats::operator +=(const SessionsStats & rhs)
{
	std::map<SessionID, SessionStats>::const_iterator itr;
	for (itr = rhs.sessionsMap.begin(); itr != rhs.sessionsMap.end(); itr++) {
		sessionsMap[itr->first] += itr->second;
	}
	
	return *this;
}

EmptySessionsStats::~EmptySessionsStats(){}
AbstractSessionsStats::~AbstractSessionsStats(){}
