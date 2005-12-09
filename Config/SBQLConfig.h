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
			SBQLConfig(string module);

			int init();
			int getBool(string param, int& value);
			int getInt(string param, int& value);
			int getDouble(string param, double& value);
			int getString(string param, string& value);
			~SBQLConfig();
	};
}
#endif
