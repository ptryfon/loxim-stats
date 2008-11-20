#include <SystemStats/ConfigStats.h>

using namespace SystemStatsLib;
/* =-=-=-=-=-=-=-=-=-=- */
ConfigOptStats::ConfigOptStats() :
	SystemStats("OPTION") {
	setKey("UNKNOWN");
	setValue("UNKNOWN");
}

void ConfigOptStats::setKey(string value) {
	setStringStats("KEY", value);
}

string ConfigOptStats::getKey() {
	return getStringStats("KEY");
}

void ConfigOptStats::setValue(string value) {
	setStringStats("VALUE", value);
}

string ConfigOptStats::getValue() {
	return getStringStats("VALUE");
}

ConfigOptStats::~ConfigOptStats() {
}

/* =-=-=-=-=-=-=-=-=-=- */

ConfigModuleStats::ConfigModuleStats() :
	SystemStats("MODULE") {
	setModuleName("UNKNOWN");
}

void ConfigModuleStats::setModuleName(string value) {
	setStringStats("MODULE_NAME", value);
}

string ConfigModuleStats::getModuleName() {
	return getStringStats("MODULE_NAME");
}

void ConfigModuleStats::addConfigOptStats(string key, ConfigOptStats* value) {
	setStatsStats(key, value);
}

ConfigOptStats* ConfigModuleStats::getConfigOptStats(string key) {
	return dynamic_cast<ConfigOptStats*>(getStatsStats(key));

}

void ConfigModuleStats::removeConfigOptStats(string key) {
	removeStats(key);
}

ConfigModuleStats::~ConfigModuleStats() {
}

/* =-=-=-=-=-=-=-=-=-=- */

ConfigsStats::ConfigsStats() :
	SystemStats("CONFIGS") {

}

void ConfigsStats::addConfigModuleStats(string key, ConfigModuleStats* value) {
	setStatsStats(key, value);
}

ConfigModuleStats* ConfigsStats::getConfigModuleStats(string key) {
	return dynamic_cast<ConfigModuleStats*>(getStatsStats(key));
}

void ConfigsStats::removeConfigModuleStats(string key) {
	removeStats(key);
}

ConfigsStats::~ConfigsStats() {

}
