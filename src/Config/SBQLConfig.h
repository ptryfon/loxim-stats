#ifndef _SBQLCONFIG_H
#define _SBQLCONFIG_H

#include <string>

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
			std::string callerModule;
			mutable struct ModuleOptions *callerModuleOpts;

			struct ModuleOptions *findConfigModule(const std::string &module) const;
			struct ConfOpt *findConfigOption(const std::string &option) const;
		public:
			SBQLConfig(const std::string &module);

			static void startup();
			int init(void);
			void free(void);
			
			int getBool(const std::string &param, bool& value) const;
			int getInt(const std::string &param, int& value) const;
			int getLong(const std::string &param, long long& value) const;
			int getDouble(const std::string &param, double& value) const;
			int getString(const std::string &param, std::string& value) const;

			bool getBoolDefault(const std::string &param, bool def) const;
			int getIntDefault(const std::string &param, int def) const;
			long long getLongDefault(const std::string &param, long long def) const;
			double getDoubleDefault(const std::string &param, double def) const;
			std::string getStringDefault(const std::string &param, const std::string& def) const;

			void dumpConfig(void);
			~SBQLConfig();
	};
}
#endif
