#ifndef _ERRORCONSOLE_H
#define _ERRORCONSOLE_H

#include <Config/SBQLConfig.h>
#include <fstream>
#include <string>
#include <map>
#include <memory>

namespace Errors {

	//the levels are monotonic, which means that V_DEBUG includes all
	enum VerbosityLevel{
		V_SEVERE_ERROR = 1,
		V_ERROR,
		V_WARNING,
		V_INFO,
		V_DEBUG
	};

	static const VerbosityLevel V_DEFAULT = V_WARNING;

	// Which level should we assume for messages, which don't use the
	// levels? I chose this one in order not to scare the user with huge
	// amount warnings/information/errors, but I am not sure if this is the
	// right choice.
	static const VerbosityLevel V_DEPRECATED = V_DEBUG;

	class ErrorConsole;

	class ErrorConsoleAdapter {
		protected:
			ErrorConsole &cons;
			VerbosityLevel l;
		public:
			ErrorConsoleAdapter(ErrorConsole &, VerbosityLevel);
			ErrorConsoleAdapter& operator<<(int error);
			ErrorConsoleAdapter& operator<<(const std::string &errorMsg);
			ErrorConsoleAdapter& printf(const char *format, ...);
	};

	class ErrorConsole {
		friend class ErrorConsoleAdapter;
		private:
			static bool serr;
                        static bool use_log_file;
			static std::auto_ptr<std::ofstream> console_file;
        		static std::auto_ptr<Config::SBQLConfig> conf_file;
			static pthread_mutex_t write_lock;
			static void init_static();
			static VerbosityLevel parse_verbosity(const string &property);
		protected:
			std::string owner;
			static std::map<std::string, ErrorConsole* > modules;
			static const Config::SBQLConfig &get_config();
			void put_string(const std::string &,
					VerbosityLevel l = V_DEPRECATED);
			void put_errno(int error, VerbosityLevel l =
					V_DEPRECATED);
			
			ErrorConsole(const std::string &module);
		public:
			static ErrorConsole &get_instance(const std::string &module);

			ErrorConsole& operator<<(int error) __attribute__
				((deprecated));
			ErrorConsole& operator<<(const std::string &errorMsg)
				__attribute__ ((deprecated));
			ErrorConsole& printf(const char *format, ...)
				__attribute__ ((deprecated));
			
			ErrorConsoleAdapter operator()(VerbosityLevel l);

			~ErrorConsole();
	};

}
#endif
