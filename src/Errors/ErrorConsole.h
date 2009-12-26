#ifndef _ERRORCONSOLE_H
#define _ERRORCONSOLE_H

#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <memory>
#include <config.h>
#include <Util/Concurrency.h>

#define EC_V_SEVERE_ERROR 1
#define EC_V_ERROR 2
#define EC_V_WARNING 3
#define EC_V_INFO 4
#define EC_V_DEBUG 5

#define debug_printf(ec, ...) { const int error_level = HAVE_VERBOSITY_COMPILE; if (error_level == EC_V_DEBUG) {(ec).do_not_use_directly_printf(::Errors::V_DEBUG, __VA_ARGS__);}}
#define debug_print(ec, msg) { const int error_level = HAVE_VERBOSITY_COMPILE; if (error_level == EC_V_DEBUG) {(ec).do_not_use_directly_stream(::Errors::V_DEBUG) << msg;}}

#define info_printf(ec, ...) { const int error_level = HAVE_VERBOSITY_COMPILE; if (error_level >= EC_V_INFO) {(ec).do_not_use_directly_printf(::Errors::V_INFO, __VA_ARGS__);}}
#define info_print(ec, msg) { const int error_level = HAVE_VERBOSITY_COMPILE; if (error_level >= EC_V_INFO) {(ec).do_not_use_directly_stream(::Errors::V_INFO) << msg;}}

#define warning_printf(ec, ...) { const int error_level = HAVE_VERBOSITY_COMPILE; if (error_level >= EC_V_WARNING) {(ec).do_not_use_directly_printf(::Errors::V_WARNING, __VA_ARGS__);}}
#define warning_print(ec, msg) { const int error_level = HAVE_VERBOSITY_COMPILE; if (error_level >= EC_V_WARNING) {(ec).do_not_use_directly_stream(::Errors::V_WARNING) << msg;}}

#define error_printf(ec, ...) { const int error_level = HAVE_VERBOSITY_COMPILE; if (error_level >= EC_V_ERROR) {(ec).do_not_use_directly_printf(::Errors::V_ERROR, __VA_ARGS__);}}
#define error_print(ec, msg) { const int error_level = HAVE_VERBOSITY_COMPILE; if (error_level >= EC_V_ERROR) {(ec).do_not_use_directly_stream(::Errors::V_ERROR) << msg;}}

#define severe_printf(ec, ...) { const int error_level = HAVE_VERBOSITY_COMPILE; if (error_level >= EC_V_SEVERE_ERROR) {(ec).do_not_use_directly_printf(::Errors::V_SEVERE_ERROR, __VA_ARGS__);}}
#define severe_print(ec, msg) { const int error_level = HAVE_VERBOSITY_COMPILE; if (error_level >= EC_V_SEVERE_ERROR) {(ec).do_not_use_directly_stream(::Errors::V_SEVERE_ERROR) << msg;}}

namespace Config {
	class SBQLConfig;
}

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
		EC_STATS,
		EC_STORE,
		EC_STORE_CLASSES,
		EC_STORE_FILE,
		EC_STORE_INTERFACES,
		EC_STORE_NAMED_ITEMS,
		EC_STORE_NAMED_ROOTS,
		EC_STORE_SCHEMAS,
		EC_STORE_VIEWS,
		EC_TRANSACTION_MANAGER,
		EC_TYPE_CHECKER,
		EC_UTIL
	};

	static const VerbosityLevel V_DEFAULT = V_WARNING;

	class ErrorConsole {
		private:
			static bool serr;
                        static bool use_log_file;
			static std::auto_ptr<std::ofstream> console_file;
			
			static std::auto_ptr<Config::SBQLConfig> conf_file;
			static Util::Mutex write_lock;
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
			void print_string(VerbosityLevel l, const std::string &msg);
		public:

			class ErrorConsoleStream
			{
				friend class ErrorConsole;
				
			public:
				inline ErrorConsoleStream(const ErrorConsoleStream &es);
				template<typename T>
				inline ErrorConsoleStream &operator<<(const T&);
				inline ~ErrorConsoleStream();
			private:
				inline ErrorConsoleStream(ErrorConsole &ec, VerbosityLevel level);
				ErrorConsole &ec;
				std::stringstream buf;
				VerbosityLevel level;
			};

			static ErrorConsole &get_instance(ConsoleInstance module);

			void do_not_use_directly_printf(VerbosityLevel l, const char *format, ...);
			inline ErrorConsoleStream do_not_use_directly_stream(VerbosityLevel l);
			
			~ErrorConsole();
	};
	ErrorConsole::ErrorConsoleStream::ErrorConsoleStream(
		ErrorConsole &ec, VerbosityLevel level): ec(ec), level(level)
	{
	}
	
	ErrorConsole::ErrorConsoleStream::ErrorConsoleStream(const ErrorConsoleStream &es):
	   ec(es.ec), level(es.level)
	{
	}

	ErrorConsole::ErrorConsoleStream::~ErrorConsoleStream()
	{
		ec.print_string(level, buf.str());
	}

	template<typename T>
	ErrorConsole::ErrorConsoleStream &ErrorConsole::ErrorConsoleStream::operator<<(const T &msg)
	{
		buf << msg;
		return *this;
	}

	ErrorConsole::ErrorConsoleStream ErrorConsole::do_not_use_directly_stream(VerbosityLevel l)
	{
		return ErrorConsoleStream(*this, l);
	}


}
#endif
