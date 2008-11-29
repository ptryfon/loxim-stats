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
			static struct ModuleOptions *config;
			string callerModule;
			mutable struct ModuleOptions *callerModuleOpts;

			struct ModuleOptions *findConfigModule(string module) const;
			struct ConfOpt *findConfigOption(string option) const;
		public:
			SBQLConfig(string module);

			static void startup();
			int init(void);
			void free(void);
			int getBool(string param, bool& value) const;
			int getInt(string param, int& value) const;
			int getLong(string param, long long& value) const;
			int getDouble(string param, double& value) const;
			int getString(string param, string& value) const;
			void dumpConfig(void);
			~SBQLConfig();
	};
}
#endif
