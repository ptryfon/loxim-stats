#include "SBQLConfig.h"

using namespace std;

namespace Config {
	struct ConfOpt {
		string name;
		string value;
		struct ConfOpt *nextOpt;
	};

	struct ModuleOptions {
		string name;
		struct ConfOpt *options;
	};

	ifstream* SBQLConfig::configFile = NULL;
	int SBQLConfig::nObjects = 0;

	SBQLConfig::SBQLConfig(string module)
	{
		nObjects++;
	};

	int SBQLConfig::init()
	{
		if (configFile == NULL)
			configFile = new ifstream("sbql.conf");

//		while () {
//		}

		return 0;
	};

	int SBQLConfig::getBool(string param, int& value)
	{
		value = 1;
		return 0;
	};
	
	int SBQLConfig::getInt(string param, int& value)
	{
		value = 666;
		return 0;
	};
	
	int SBQLConfig::getDouble(string param, double& value)
	{
		value = 6.66;
		return 0;
	};
	
	int SBQLConfig::getString(string param, string& value)
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
