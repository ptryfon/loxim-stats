/*
 * SystemStats.h
 *
 *  Created on: 27-cze-08
 *      Author: damianklata
 */

#ifndef SYSTEMSTATS_H_
#define SYSTEMSTATS_H_

#include <iostream>
#include <map>
#include <string>

using namespace std;

namespace SystemStatsLib{
	/* This class contains value or set of values of
	 * specified type. Now there exists 4 subclasses
	 * that may contains string, int, double or set of values.
	 */
	class StatsValue;


	/*
	 * Base class for statistics. This class contains
	 * map of values. Pair of string key (unique) and
	 * value. All statistic that we whan't to write must
	 * inherits from this class. Because store have
	 * quite abstract mechanism to present stats as
	 * result of loxim query. Example:
	 *
	 * class ConfigOptStats: public SystemStats {
	 *
     * public:
	 *   ConfigOptStats();
	 *
	 *	 void setKey(string value);
	 *	 string getKey();
     *
	 *	 void setValue(string value);
	 *	 string getValue();
     *
	 *	 ~ConfigOptStats();
	 * };
	 *
	 * See: ConfigStats.cpp
	 */
	class SystemStats{
		protected:
			map<string, StatsValue*> mapOfValues;
			string name;
		public:
			SystemStats(string name);

			string getName();

			int getIntStats(string name);
			void setIntStats(string name, int value);

			double getDoubleStats(string name);
			void setDoubleStats(string name, double value);

			string getStringStats(string name);
			void setStringStats(string name, string value);

			SystemStats* getStatsStats(string name);
			void setStatsStats(string name, SystemStats* value);

			void removeStats(string name);

			map<string, StatsValue*> getAllStats();
			virtual ~SystemStats();

			virtual void refreshStats();
	};

	/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
	/* Base class and 4 subclasses that contains
	 * values in map of statistics
	 */

	class StatsValue {
	protected:
			string key;
	public:
			StatsValue(string key);
			virtual ~StatsValue();
			virtual int getType();
			string getKey();
	};

	class IntStatsValue: public StatsValue{
	private:
			int value;
	public:
		IntStatsValue(string key);
		int getType();
		void setValue(int value);
		int getValue();
	};

	class DoubleStatsValue: public StatsValue{
	private:
			double value;
	public:
		DoubleStatsValue(string key);
		int getType();
		void setValue(double value);
		double getValue();
	};

	class StringStatsValue: public StatsValue{
	private:
			string value;
	public:
		StringStatsValue(string key);
		int getType();
		void setValue(string value);
		string getValue();
	};

	class StatsStatsValue: public StatsValue{
	private:
			SystemStats* value;
	public:
		StatsStatsValue(string key);
		int getType();
		void setValue(SystemStats* value);
		SystemStats* getValue();
		~StatsStatsValue();
	};

}
#endif /* SYSTEMSTATS_H_ */
