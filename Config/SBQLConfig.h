#ifndef _SBQLCONFIG_H
#define _SBQLCONFIG_H

#include <string>
#include <fstream>

using namespace std;

namespace Config {
	class SBQLConfig {
		private:
			static ifstream* configFile;
			static int nObjects;
		public:
			SBQLConfig();
			SBQLConfig(char *module);

			int init();
			int getBool(char* param, int& value);
			int getInt(char* param, int& value);
			int getDouble(char* param, double& value);
			int getString(char* param, string& value);
			~SBQLConfig();
	};
}
#endif
