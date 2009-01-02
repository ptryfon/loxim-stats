#ifndef _ERRORCONSOLE_H
#define _ERRORCONSOLE_H

#include <Config/SBQLConfig.h>
#include <fstream>
#include <string>
#include <map>
#include <memory>
#include <config.h>

#define EC_V_SEVERE_ERROR 1
#define EC_V_ERROR 2
#define EC_V_WARNING 3
#define EC_V_INFO 4
#define EC_V_DEBUG 5


#if HAVE_VERBOSITY_COMPILE == EC_V_DEBUG
#define debug_printf(ec, fmt, ...) (ec)(Errors::V_DEBUG).printf((fmt), __VA_ARGS__)
#define debug_print(ec, msg) (ec)(Errors::V_DEBUG).print(msg)
#else
#define debug_printf(ec, fmt, ...)
#define debug_print(ec, msg)
#endif

#if HAVE_VERBOSITY_COMPILE >= EC_V_INFO
#define info_printf(ec, fmt, ...) (ec)(Errors::V_INFO).printf((fmt), __VA_ARGS__)
#define info_print(ec, msg) (ec)(Errors::V_INFO).print(msg)
#else
#define info_printf(ec, fmt, ...)
#define info_print(ec, msg)
#endif

#if HAVE_VERBOSITY_COMPILE >= EC_V_WARNING
#define warning_printf(ec, fmt, ...) (ec)(Errors::V_WARNING).printf((fmt), __VA_ARGS__)
#define warning_print(ec, msg) (ec)(Errors::V_WARNING).print(msg)
#else
#define warning_printf(ec, fmt, ...)
#define warning_print(ec, msg)
#endif

#if HAVE_VERBOSITY_COMPILE >= EC_V_ERROR
#define error_printf(ec, fmt, ...) (ec)(Errors::V_ERROR).printf((fmt), __VA_ARGS__)
#define error_print(ec, msg) (ec)(Errors::V_ERROR).print(msg)
#else
#define error_printf(ec, fmt, ...)
#define error_print(ec, msg)
#endif

#if HAVE_VERBOSITY_COMPILE >= EC_V_SEVERE_ERROR
#define severe_printf(ec, fmt, ...) (ec)(Errors::V_SEVERE_ERROR).printf((fmt), __VA_ARGS__)
#define severe_print(ec, msg) (ec)(Errors::V_SEVERE_ERROR).printf(msg)
#else
#define severe_printf(ec, fmt, ...)
#define severe_print(ec, msg)
#endif

namespace Errors {

	//the levels are monotonic, which means that V_DEBUG includes all
	enum VerbosityLevel{
		V_SEVERE_ERROR = EC_V_SEVERE_ERROR,
		V_ERROR = EC_V_ERROR,
		V_WARNING = EC_V_WARNING,
		V_INFO = EC_V_INFO,
		V_DEBUG = EC_V_DEBUG
	};

	//if you modify these, please make sure you modify the appropriate
	//descriptions in names definition in cpp file. Watch out for the order.
	//Such a solution is used to increase performance, because we use the
	//error console pretty much.
	enum ConsoleInstance{
		EC_ARIES_LOG = 0,
		EC_INDEX_MANAGER,
		EC_INTERFACE_MAPS,
		EC_LOCK_MANAGER,
		EC_LOG,
		EC_OUTER_SCHEMAS,
		EC_QUERY_EXECUTOR,
		EC_QUERY_PARSER,
		EC_SERVER,
		EC_STORE,
		EC_STORE_CLASSES,
		EC_STORE_FILE,
		EC_STORE_INTERFACES,
		EC_STORE_NAMED_ITEMS,
		EC_STORE_NAMED_ROOTS,
		EC_STORE_SCHEMAS,
		EC_STORE_VIEWS,
		EC_TRANSACTION_MANAGER,
		EC_TYPE_CHECKER
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
			void printf(const char *format, ...);
			void print(const std::string &msg);
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
			static const string &verbosity_name(VerbosityLevel l);
		protected:
			ConsoleInstance instance;
			static std::map<ConsoleInstance, ErrorConsole* > modules;
			static const string names[];
			static const string verb_debug_name;
			static const string verb_info_name;
			static const string verb_warning_name;
			static const string verb_error_name;
			static const string verb_severe_error_name;
			static const Config::SBQLConfig &get_config();
			void put_string(const std::string &,
					VerbosityLevel l = V_DEPRECATED);
			void put_errno(int error, VerbosityLevel l =
					V_DEPRECATED);
			
			ErrorConsole(ConsoleInstance module);
		public:
			static ErrorConsole &get_instance(ConsoleInstance module);

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
