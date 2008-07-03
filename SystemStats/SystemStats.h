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
	class StatsValue;

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
	};

	class StatsValue {
	protected:
			string key;
	public:
			StatsValue(string key);
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
	};

}
#endif /* SYSTEMSTATS_H_ */
