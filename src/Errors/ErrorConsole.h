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
#define debug_printf(ec, ...) (ec).do_not_use_directly_printf(::Errors::V_DEBUG, __VA_ARGS__)
#define debug_print(ec, msg) (ec).do_not_use_directly_print(::Errors::V_DEBUG, msg)
#else
#define debug_printf(ec, ...)
#define debug_print(ec, msg)
#endif

#if HAVE_VERBOSITY_COMPILE >= EC_V_INFO
#define info_printf(ec, ...) (ec).do_not_use_directly_printf(::Errors::V_INFO, __VA_ARGS__)
#define info_print(ec, msg) (ec).do_not_use_directly_print(::Errors::V_INFO, msg)
#else
#define info_printf(ec, ...)
#define info_print(ec, msg)
#endif

#if HAVE_VERBOSITY_COMPILE >= EC_V_WARNING
#define warning_printf(ec, ...) (ec).do_not_use_directly_printf(::Errors::V_WARNING, __VA_ARGS__)
#define warning_print(ec, msg) (ec).do_not_use_directly_print(::Errors::V_WARNING, msg)
#else
#define warning_printf(ec, ...)
#define warning_print(ec, msg)
#endif

#if HAVE_VERBOSITY_COMPILE >= EC_V_ERROR
#define error_printf(ec, ...) (ec).do_not_use_directly_printf(::Errors::V_ERROR, __VA_ARGS__)
#define error_print(ec, msg) (ec).do_not_use_directly_print(::Errors::V_ERROR, msg)
#else
#define error_printf(ec, ...)
#define error_print(ec, msg)
#endif

#if HAVE_VERBOSITY_COMPILE >= EC_V_SEVERE_ERROR
#define severe_printf(ec, ...) (ec).do_not_use_directly_printf(::Errors::V_SEVERE_ERROR, __VA_ARGS__)
#define severe_print(ec, msg) (ec).do_not_use_directly_printf(::Errors::V_SEVERE_ERROR, msg)
#else
#define severe_printf(ec, ...)
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

	class ErrorConsole {
		friend class ErrorConsoleAdapter;
		private:
			static bool serr;
                        static bool use_log_file;
			static std::auto_ptr<std::ofstream> console_file;
        		static std::auto_ptr<Config::SBQLConfig> conf_file;
			static pthread_mutex_t write_lock;
			static void init_static();
			static VerbosityLevel parse_verbosity(const std::string &property);
			static const std::string &verbosity_name(VerbosityLevel l);
		protected:
			ConsoleInstance instance;
			static std::map<ConsoleInstance, ErrorConsole* > modules;
			static const std::string names[];
			static const std::string verb_debug_name;
			static const std::string verb_info_name;
			static const std::string verb_warning_name;
			static const std::string verb_error_name;
			static const std::string verb_severe_error_name;
			static const Config::SBQLConfig &get_config();
			
			ErrorConsole(ConsoleInstance module);
		public:
			static ErrorConsole &get_instance(ConsoleInstance module);

			void do_not_use_directly_print(VerbosityLevel l, const std::string &msg);
			void do_not_use_directly_print(VerbosityLevel l, int error);
			void do_not_use_directly_printf(VerbosityLevel l, const char *format, ...);
			
			~ErrorConsole();
	};

}
#endif