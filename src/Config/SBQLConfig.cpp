#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <Errors/Errors.h>
#include <Config/SBQLConfig.h>
#include <SystemStats/ConfigStats.h>
#include <SystemStats/AllStats.h>
#include <unistd.h>
#include <pwd.h>

using namespace std;
using namespace Errors;
using namespace SystemStatsLib;

namespace Config {
	struct ModuleOptions *SBQLConfig::config = NULL;

	void SBQLConfig::startup() {
		config = NULL;
	}

	SBQLConfig::SBQLConfig(string module)
	{
		callerModule = module;
		callerModuleOpts = findConfigModule(callerModule);
	};

	int SBQLConfig::init()
	{
		ifstream configFile;
		char *c1, *c2, *n, *v;
		char line[256];
		struct ModuleOptions *mod, *lastmod;
		struct ConfOpt *opt, *lastopt;
		struct passwd *passwd = getpwuid(geteuid());
		if (passwd){
			string file_name = string(passwd->pw_dir) + "/.loxim.conf";
			if (!access(file_name.c_str(), R_OK))
				configFile.open(file_name.c_str());
		}
		if (!configFile.is_open())
			configFile.open("/etc/loxim.conf");

		if (!configFile.is_open())
			return ErrConfig | ENoFile;

		while (!configFile.eof()) {
			configFile.getline(line, 255);
			line[255] = '\0';
			// clear comments (remove everything after and including '#')
			for (unsigned int i=0; i<strlen(line); i++) {
				if (line[i] == '#') {
					line[i] = '\0';
					break;
				}
			}
			n = line;
			// remove starting whitespace
			while ((strlen(n) > 0) && (*n == ' ' || *n == '\t'))
				n++;
			// ignore empty lines
			if (strlen(n) == 0)
				continue;
			// remove trailing whitespace
			v = n + strlen(n) - 1;
			while (*v == ' ' || *v == '\t') {
				*v = '\0';
				v--;
			}
			// check for [.*] pattern
			c1 = strchr(n,'[');
			if (c1 != NULL) {
				c1++;
				// Module name
				c2 = strchr(c1, ']');
				if (c2 == NULL) {
					free();
					configFile.close();
					return ErrConfig | ENotParsed;
				}
				*c2 = '\0';
				mod = config;
				while (mod != NULL) {
					if (strcasecmp(mod->name, c1) == 0)
						break;
					mod = mod->nextMod;
				}
				if (mod == NULL) {
					mod = (struct ModuleOptions *)malloc(sizeof(struct ModuleOptions));
					if (mod == NULL) {
						free();
						configFile.close();
						return ErrConfig | ENOMEM;
					}
					mod->name = strdup(c1);
					mod->options = NULL;
					mod->nextMod = NULL;
					if (config == NULL) {
						config = mod;
					} else {
						lastmod->nextMod = mod;
					}
					lastmod = mod;
					lastopt = NULL;
				} else {
					lastopt = mod->options;
					while (lastopt != NULL) {
						if (lastopt->nextOpt != NULL)
							lastopt = lastopt->nextOpt;
						else
							break;
					}
				}
				continue;
			}
			// check for name = value pattern
			for (unsigned int i=0; i<strlen(n); i++) {
				if (*(n + i) == ' ' || *(n + i) == '\t' || *(n + i) == '=') {
					v = n + i + 1;
					*(n + i) = '\0';
					break;
				}
			}
			if (strlen(n) == 0) {
				free();
				configFile.close();
				return ErrConfig | ENotParsed;
			}
			while ((strlen(v) > 0) && (*v == ' ' || *v == '\t' || *v == '='))
				v++;
			if (strlen(v) == 0 || config == NULL) {
				free();
				configFile.close();
				return ErrConfig | ENotParsed;
			}
			opt = mod->options;
			while (opt != NULL) {
				if (strcasecmp(opt->name, n) == 0)
					break;
				opt = opt->nextOpt;
			}
			if (opt == NULL) {
				opt = (struct ConfOpt *)malloc(sizeof(struct ConfOpt));
				if (opt == NULL) {
					free();
					configFile.close();
					return ErrConfig | ENOMEM;
				}
				opt->name = strdup(n);
				opt->nextOpt = NULL;
				if (lastopt == NULL)
					mod->options = opt;
				else
					lastopt->nextOpt = opt;
				lastopt = opt;
			}
			opt->value = strdup(v);
		}

		configFile.close();

		/* Add config to stats */
		ConfigsStats* cs = AllStats::getHandle()->getConfigsStats();
		ModuleOptions* module = config;
		while (module != NULL) {
			ConfOpt* opt = module->options;
			ConfigModuleStats* cms = new ConfigModuleStats();
			cms->setModuleName(module->name);
			while (opt != NULL) {
				ConfigOptStats* cos = new ConfigOptStats();
				cos->setKey(opt->name);
				cos->setValue(opt->value);
				cms->addConfigOptStats(opt->name, cos);
				opt = opt->nextOpt;
			}
			cs->addConfigModuleStats(module->name, cms);
			module = module->nextMod;
		}
		return 0;
	};

	struct ModuleOptions *SBQLConfig::findConfigModule(string module) const
	{
		struct ModuleOptions *mod;

		if (config != NULL) {
			mod = config;
			while (mod != NULL) {
				if (strcasecmp(mod->name, module.c_str()) == 0) {
					return mod;
				}
				mod = mod->nextMod;
			}
		}
		return NULL;
	};

	struct ConfOpt *SBQLConfig::findConfigOption(string option) const
	{
		struct ConfOpt *opt;

		if (callerModuleOpts == NULL)
			callerModuleOpts = findConfigModule(callerModule);

		if (callerModuleOpts == NULL)
			return NULL;

		opt = callerModuleOpts->options;
		while (opt != NULL) {
			if (strcasecmp(opt->name, option.c_str()) == 0) {
				return opt;
			}
			opt = opt->nextOpt;
		}
		return NULL;
	};

	void SBQLConfig::dumpConfig(void)
	{
		struct ModuleOptions *mod;
		struct ConfOpt *opt;

		mod = config;
		while (mod != NULL) {
			cout << "[" << mod->name << "]" << endl;
			opt = mod->options;
			while (opt != NULL) {
				cout << opt->name << " = " << opt->value << endl;
				opt = opt->nextOpt;
			}
			cout << endl;
			mod = mod->nextMod;
		}
	};

	int SBQLConfig::getBool(string param, bool& value) const
	{
		struct ConfOpt *opt;

		if (config == NULL)
			return ErrConfig | ENotInit;

		opt = findConfigOption(param);
		if (opt == NULL)
			return ErrConfig | ENoValue;

		if ((strcasecmp(opt->value, "yes") == 0) ||
				(strcasecmp(opt->value, "1") == 0) ||
				(strcasecmp(opt->value, "on") == 0) ||
				(strcasecmp(opt->value, "true") == 0)) {
			value = true;
		} else {
			if ((strcasecmp(opt->value, "no") == 0) ||
					(strcasecmp(opt->value, "0") == 0) ||
					(strcasecmp(opt->value, "off") == 0) ||
					(strcasecmp(opt->value, "false") == 0)) {
				value = false;
			} else {
				return ErrConfig | EBadValue;
			}
		}

		return 0;
	};

	int SBQLConfig::getInt(string param, int& value) const
	{
		struct ConfOpt *opt;

		if (config == NULL)
			return ErrConfig | ENotInit;

		opt = findConfigOption(param);
		if (opt == NULL)
			return ErrConfig | ENoValue;

		value = strtol(opt->value, NULL, 10);
		if (errno == ERANGE || errno == EINVAL)
			return ErrConfig || EBadValue;

		return 0;
	};

	int SBQLConfig::getLong(string param, long long& value) const
	{
		struct ConfOpt *opt;

		if (config == NULL)
			return ErrConfig | ENotInit;

		opt = findConfigOption(param);
		if (opt == NULL)
			return ErrConfig | ENoValue;

		value = strtoll(opt->value, NULL, 10);
		if (errno == ERANGE || errno == EINVAL)
			return ErrConfig || EBadValue;

		return 0;
	};

	int SBQLConfig::getDouble(string param, double& value) const
	{
		struct ConfOpt *opt;

		if (config == NULL)
			return ErrConfig | ENotInit;

		opt = findConfigOption(param);
		if (opt == NULL)
			return ErrConfig | ENoValue;

		value = strtod(opt->value, NULL);
		if (errno == ERANGE || errno == EINVAL)
			return ErrConfig || EBadValue;

		return 0;
	};

	int SBQLConfig::getString(string param, string& value) const
	{
		struct ConfOpt *opt;

		if (config == NULL)
			return ErrConfig | ENotInit;

		opt = findConfigOption(param);
		if (opt == NULL)
			return ErrConfig | ENoValue;

		value = opt->value;
		return 0;
	};

	void SBQLConfig::free(void)
	{
		struct ModuleOptions *mod, *pmod;
		struct ConfOpt *opt, *popt;

		mod = config;
		while (mod != NULL) {
			opt = mod->options;
			while (opt != NULL) {
				popt = opt;
				opt = opt->nextOpt;
				::free(popt);
			}
			pmod = mod;
			mod = mod->nextMod;
			::free(pmod);
		}
		config = NULL;
	};

	SBQLConfig::~SBQLConfig()
	{
	};
}
