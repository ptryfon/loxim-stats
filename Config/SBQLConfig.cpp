#include "SBQLConfig.h"

using namespace std;

namespace Config {
	ifstream* SBQLConfig::configFile = NULL;
	int SBQLConfig::nObjects = 0;

	int SBQLConfig::init()
	{
		return 0;
	};

	SBQLConfig::SBQLConfig()
       	{
		if (configFile == NULL)
			configFile = new ifstream("sbql.conf");
		nObjects++;
	};

	SBQLConfig::SBQLConfig(char *module)
	{
		nObjects++;
	};

	int SBQLConfig::getBool(char* param, int& value)
	{
		value = 1;
		return 0;
	};
	
	int SBQLConfig::getInt(char* param, int& value)
	{
		value = 666;
		return 0;
	};
	
	int SBQLConfig::getDouble(char* param, double& value)
	{
		value = 6.66;
		return 0;
	};
	
	int SBQLConfig::getString(char* param, string& value)
	{
		value = "666";
		return 0;
	};
	
	SBQLConfig::~SBQLConfig()
	{
		nObjects--;
		if (nObjects == 0) {
			delete configFile;
			configFile = NULL;
		}
	};
}
