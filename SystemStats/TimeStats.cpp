/*
 * TimeStats.cpp
 *
 *  Created on: 11-lip-08
 *      Author: damianklata
 */

#include "TimeStats.h"

TimeStats::TimeStats() {
	startTime = time();
	sum = 0;
	count = 0;
	index = 0;
}

TimeStats::~TimeStats() {
}

void TimeStats::add(int value) {
	currentTime = time();
	double dif = difftime(currentTime, startTime);
	int mindif = int(dif / 60);
	int writeidx = index;
	while (mindif > 0) {
		if (count < 60) {
			count++;
		} else {
			sum = sum - values[index];
			index++;
			index = index % 60;
		}
		writeidx++;
		writeidx = writeidx % 60;
		values[writeidx] = 0;
		mindif--;
	}
	values[index] += value;
	sum += value;
}

int TimeStats::getSum() {
	add(0);
	return sum;
}
