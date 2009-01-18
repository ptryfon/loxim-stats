/*
 * SystemStats.cpp
 *
 *  Created on: 27-cze-08
 *      Author: damianklata
 */
#include <SystemStats/SystemStats.h>

using namespace SystemStatsLib;

SystemStats::SystemStats(string name) : name(name) {
}

string SystemStats::getName() {
	return name;
}

int SystemStats::getIntStats(const string &name) {
	map<string, StatsValue*>::iterator cur = mapOfValues.find(name);
		if (cur != mapOfValues.end()) {
			return dynamic_cast<IntStatsValue*>(mapOfValues[name])->getValue();
		} else {
			return 0;
		}
}

void SystemStats::setIntStats(const string &name, int value) {
	map<string, StatsValue*>::iterator cur = mapOfValues.find(name);
		if (cur != mapOfValues.end()) {
			dynamic_cast<IntStatsValue*>(mapOfValues[name])->setValue(value);
		} else {
			IntStatsValue* val = new IntStatsValue(name);
			val->setValue(value);
			mapOfValues[name] = val;
		}
}

double SystemStats::getDoubleStats(const string &name) {
	map<string, StatsValue*>::iterator cur = mapOfValues.find(name);
		if (cur != mapOfValues.end()) {
			return dynamic_cast<DoubleStatsValue*>(mapOfValues[name])->getValue();
		} else {
			return 0.0;
		}
}

void SystemStats::setDoubleStats(const string &name, double value) {
	map<string, StatsValue*>::iterator cur = mapOfValues.find(name);
		if (cur != mapOfValues.end()) {
			dynamic_cast<DoubleStatsValue*>(mapOfValues[name])->setValue(value);
		} else {
			DoubleStatsValue* val = new DoubleStatsValue(name);
			val->setValue(value);
			mapOfValues[name] = val;
		}
}

string SystemStats::getStringStats(const string &name) {
	map<string, StatsValue*>::iterator cur = mapOfValues.find(name);
		if (cur != mapOfValues.end()) {
			return dynamic_cast<StringStatsValue*>(mapOfValues[name])->getValue();
		} else {
			return "";
		}
}

void SystemStats::setStringStats(const string &name, string value) {
	map<string, StatsValue*>::iterator cur = mapOfValues.find(name);
		if (cur != mapOfValues.end()) {
			dynamic_cast<StringStatsValue*>(mapOfValues[name])->setValue(value);
		} else {
			StringStatsValue* val = new StringStatsValue(name);
			val->setValue(value);
			mapOfValues[name] = val;
		}
}

SystemStats* SystemStats::getStatsStats(const string &name) {
	map<string, StatsValue*>::iterator cur = mapOfValues.find(name);
		if (cur != mapOfValues.end()) {
			return dynamic_cast<StatsStatsValue*>(mapOfValues[name])->getValue();
		} else {
			return 0;
		}
}

void SystemStats::setStatsStats(const string &name, SystemStats* value) {
	map<string, StatsValue*>::iterator cur = mapOfValues.find(name);
		if (cur != mapOfValues.end()) {
			dynamic_cast<StatsStatsValue*>(mapOfValues[name])->setValue(value);
		} else {
			StatsStatsValue* val = new StatsStatsValue(name);
			val->setValue(value);
			mapOfValues[name] = val;
		}
}

void SystemStats::removeStats(const string &name) {
	mapOfValues.erase(name);
}

map<string, StatsValue*>  SystemStats::getAllStats() {
	return mapOfValues;
}

SystemStats::~SystemStats() {
	map<string, StatsValue*>::const_iterator itr;

	for(itr = mapOfValues.begin(); itr != mapOfValues.end(); ++itr){
		StatsValue* v = (*itr).second;
		delete v;
	}
}

void SystemStats::refreshStats() {
	map<string, StatsValue*>::const_iterator itr;

	for(itr = mapOfValues.begin(); itr != mapOfValues.end(); ++itr){
		StatsValue* v = (*itr).second;
		if (v->getType() == 4) {
			dynamic_cast<StatsStatsValue*>(v)->getValue()->refreshStats();
		}
	}
}

StatsValue::StatsValue(string key) {
	this->key = key;
}

StatsValue::~StatsValue() {
}

int StatsValue::getType() {
	return -1;
}

string StatsValue::getKey() {
	return key;
}

IntStatsValue::IntStatsValue(string key): StatsValue(key) {
}

int IntStatsValue::getType() {
	return 1;
}

void IntStatsValue::setValue(int value) {
	this->value = value;
}

int IntStatsValue::getValue() {
	return value;
};

DoubleStatsValue::DoubleStatsValue(string key): StatsValue(key) {
}

int DoubleStatsValue::getType() {
	return 2;
}

void DoubleStatsValue::setValue(double value) {
	this->value = value;
}

double DoubleStatsValue::getValue() {
	return value;
};

StringStatsValue::StringStatsValue(string key): StatsValue(key) {
}

int StringStatsValue::getType() {
	return 3;
}

void StringStatsValue::setValue(string value) {
	this->value = value;
}

string StringStatsValue::getValue() {
	return value;
};

StatsStatsValue::StatsStatsValue(string key): StatsValue(key) {
}

int StatsStatsValue::getType() {
	return 4;
}

void StatsStatsValue::setValue(SystemStats* value) {
	this->value = value;
}

SystemStats* StatsStatsValue::getValue() {
	return value;
};

StatsStatsValue::~StatsStatsValue() {
	delete value;
}
