#include "StoreStats.h"
#include <sstream>

using namespace SystemStatsLib;

StoreStats::StoreStats(): SystemStats("STORE") {
	diskPageReads = 0;
	pageReads = 0;
	diskPageWrites = 0;
	pageWrites = 0;
	maxspeed = 0;
	minspeed = 1000.0;
	allbytes = 0;
	allmilisec = 0;

	addDiskPageReads(0);
	addPageReads(0);
	addDiskPageWrites(0);
	addPageWrites(0);
}


void StoreStats::addDiskPageReads(int count) {
	diskPageReads += count;
	setIntStats("DISK_PAGE_READS", diskPageReads);
	double hit = 0.0;
	if (pageReads > 0) {
		hit = 100 * ((1.0 * (pageReads - diskPageReads)) / pageReads);
	}
	setDoubleStats("PAGE_READS_HIT", hit);
}

int StoreStats::getDiskPageReads() {
	return getIntStats("DISK_PAGE_READS");
}

void StoreStats::addPageReads(int count) {
	pageReads += count;
	setIntStats("PAGE_READS", pageReads);
	double hit = 0.0;
	if (pageReads > 0) {
		hit = 100 * ((1.0 * (pageReads - diskPageReads)) / pageReads);
	}
	setDoubleStats("PAGE_READS_HIT", hit);
}

int StoreStats::getPageReads() {
	return getIntStats("PAGE_READS");
}

double StoreStats::getPageReadsHit() {
	return getDoubleStats("PAGE_READS_HIT");
}

void StoreStats::addDiskPageWrites(int count) {
	diskPageWrites += count;
	setIntStats("DISK_PAGE_WRITES", diskPageWrites);
	double hit = 0.0;
	if (pageWrites > 0) {
		hit = 100 * ((1.0 * (pageWrites - diskPageWrites)) / pageWrites);
	}
	setDoubleStats("PAGE_WRITES_HIT", hit);
}

int StoreStats::getDiskPageWrites() {
	return getIntStats("DISK_PAGE_WRITES");
}

void StoreStats::addPageWrites(int count) {
	pageWrites += count;
	setIntStats("PAGE_WRITES", pageWrites);
	double hit = 0.0;
	if (pageReads > 0) {
		hit = 100 * ((1.0 * (pageWrites - diskPageWrites)) / pageWrites);
	}
	setDoubleStats("PAGE_WRITES_HIT", hit);
}

int StoreStats::getPageWrites() {
	return getIntStats("PAGE_WRITES");
}

double StoreStats::getPageWritesHit() {
	return getDoubleStats("PAGE_WRITES_HIT");
}

void StoreStats::addReadTime(int bytes, double milisec) {
	if (milisec > 0 && bytes > 0) {
		double speed = (1.0 * bytes * 1024) / (milisec * 1000);
		allbytes += bytes;
		allmilisec += milisec;

		if (speed > maxspeed) {
			maxspeed = speed;
				string max_str;
				stringstream max_ss;
						max_ss << speed;
						max_ss >> max_str;
				setStringStats("MAX_READ_SPEED", max_str + " KB/S");
		}

		if (speed < minspeed) {
			minspeed = speed;
			string min_str;
			stringstream min_ss;
					min_ss << speed;
					min_ss >> min_str;
			setStringStats("MIN_READ_SPEED", min_str + " KB/S");
		}

		double avgspeed = (1.0 * allbytes * 1024) / (allmilisec * 1000);
		string avg_str;
		stringstream avg_ss;
				avg_ss << avgspeed;
				avg_ss >> avg_str;
		setStringStats("AVG_READ_SPEED", avg_str + " KB/S");
	}

}

string StoreStats::getMinReadSpeed() {
	return getStringStats("MIN_READ_SPEED");
}

string StoreStats::getMaxReadSpeed() {
	return getStringStats("MAX_READ_SPEED");
}

string StoreStats::getAvgReadSpeed() {
	return getStringStats("AVG_READ_SPEED");
}

StoreStats::~StoreStats() {
}


