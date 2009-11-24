/*
 * DiskUsageStats.cpp
 *
 *  Created on: 12-09-2009
 *      Author: paweltryfon
 */

 #include <SystemStats/DiskUsageStats.h>
 
using namespace SystemStatsLib;

DiskUsageStats::DiskUsageStats(){
	const double initialMinSpeed = 1000.0;
	const double initialMaxSpeed = 0.0;
	diskPageReads = 0;
	pageReads = 0;
	pageReadsHit = 1.0;
	diskPageWrites = 0;
	pageWrites = 0;
	pageWritesHit = 1.0;

	readMaxSpeed = initialMinSpeed;
	readMinSpeed = initialMaxSpeed;
	readAvgSpeed = 0.0;
	readAllMilisec = 0;
	readAllBytes = 0;

	writeMaxSpeed = initialMaxSpeed;
	writeMinSpeed = initialMinSpeed;
	writeAvgSpeed = 0.0;
	writeAllMilisec = 0;
	writeAllBytes = 0;
	
	/* Possibilities of futher development:
	 * - adding standard deviation of speed
	 */
}


void DiskUsageStats::addDiskPageReads(int count) {
	diskPageReads += count;
	pageReads += count;
	updatePageReadsHit();
}

int DiskUsageStats::getDiskPageReads() const {
	return diskPageReads;
}

void DiskUsageStats::addPageReads(int count) {
	pageReads += count;
	updatePageReadsHit();
}

int DiskUsageStats::getPageReads() const{
	return pageReads;
}

double DiskUsageStats::getPageReadsHit() const {
	return pageReadsHit;
}

void DiskUsageStats::addDiskPageWrites(int count) {
	diskPageWrites += count;
	pageWrites += count;
	updatePageWritesHit();
}

int DiskUsageStats::getDiskPageWrites() const {
	return diskPageWrites;
}

void DiskUsageStats::addPageWrites(int count) {
	pageWrites += count;
	updatePageWritesHit();
}

int DiskUsageStats::getPageWrites() const {
	return pageWrites;
}

double DiskUsageStats::getPageWritesHit() const {
	return pageWritesHit;
}

void DiskUsageStats::addReadTime(int bytes, double milisec) {
	if (milisec > 0 && bytes > 0) {
		double speed = (1.0 * bytes * 1024) / (milisec * 1000);
		readAllBytes += bytes;
		readAllMilisec += milisec;

		if (speed > readMaxSpeed) {
			readMaxSpeed = speed;
		}

		if (speed < readMinSpeed) {
			readMinSpeed = speed;
		}

		readAvgSpeed = (1.0 * allBytes * 1024) / (allMilisec * 1000);
	}

}

double DiskUsageStats::getMinReadSpeed() const {
	return readMinSpeed;
}

double DiskUsageStats::getMaxReadSpeed() const {
	return readMaxSpeed;
}

double DiskUsageStats::getAvgReadSpeed() const {
	return readAvgSpeed;
}

void DiskUsageStats::addWriteTime(int bytes, double milisec) {
	if (milisec > 0 && bytes > 0) {
		double speed = (1.0 * bytes * 1024) / (milisec * 1000);
		writeAllBytes += bytes;
		writeAllMilisec += milisec;

		if (speed > writeMaxSpeed) {
			writeMaxSpeed = speed;
		}

		if (speed < writeMinSpeed) {
			writeMinSpeed = speed;
		}

		writeAvgSpeed = (1.0 * allBytes * 1024) / (allMilisec * 1000);
	}

}

double DiskUsageStats::getMinWriteSpeed() const {
	return writeMinSpeed;
}

double DiskUsageStats::getMaxWriteSpeed() const {
	return writeMaxSpeed;
}

double DiskUsageStats::getAvgWriteSpeed() const {
	return writeAvgSpeed;
}

void DiskUsageStats::updatePageReadsHit() {
	if (pageReads > 0) {
		pageReadsHit = (1.0 * (pageReads - diskPageReads)) / pageReads;
	}
}

void DiskUsageStats::updatePageWritesHit() {
	if (pageWrites > 0) {
		pageWritesHit = (1.0 * (pageWrites - diskPageWrites)) / pageWrites;
	}
}

DiskUsageStats::~DiskUsageStats() {
}
