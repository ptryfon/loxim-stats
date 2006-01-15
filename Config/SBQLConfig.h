#ifndef _SBQLCONFIG_H
#define _SBQLCONFIG_H

#include <string>
#include <fstream>

using namespace std;

namespace Config {
	struct ConfOpt {
		char *name;
		char *value;
		struct ConfOpt *nextOpt;
	};

	struct ModuleOptions {
		char *name;
		struct ConfOpt *options;
		struct ModuleOptions *nextMod;
	};

	class SBQLConfig {
		private:
			static int nObjects;
			static struct ModuleOptions *config;
			string callerModule;
			struct ModuleOptions *callerModuleOpts;

			void freeConfig(void);
			struct ModuleOptions *findConfigModule(string module);
			struct ConfOpt *findConfigOption(string option);
		public:
			SBQLConfig(string module);

			int init(void);
			int init(string file);
			int getBool(string param, bool& value);
			int getInt(string param, int& value);
			int getLong(string param, long long& value);
			int getDouble(string param, double& value);
			int getString(string param, string& value);
			void dumpConfig(void);
			~SBQLConfig();
	};
}
#endif
