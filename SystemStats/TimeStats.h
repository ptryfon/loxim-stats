/*
 * TimeStats.h
 *
 *  Created on: 11-lip-08
 *      Author: damianklata
 */

#ifndef TIMESTATS_H_
#define TIMESTATS_H_

#include <time>

using namespace std;

namespace SystemStatsLib {
	class TimeStats {
	protected:
		int mapOfValues[60]; /* 1 HOUR -> INTERVAL = 1 MIN */
		int index;
		int count;
		int sum;
		time_t startTime;
	public:
		TimeStats();
		virtual ~TimeStats();

		void add(int value);
		int getSum();
	};

}

class TimeStats
{
public:
TimeStats();
virtual ~TimeStats();
};

#endif /* TIMESTATS_H_ */
