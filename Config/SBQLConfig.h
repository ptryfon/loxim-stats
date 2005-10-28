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

			virtual int init();
			virtual int getBool(char* param, int& value);
			virtual int getInt(char* param, int& value);
			virtual int getDouble(char* param, double& value);
			virtual int getString(char* param, string& value);
			virtual ~SBQLConfig();
	};
}
