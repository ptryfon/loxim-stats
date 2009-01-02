#include <stdarg.h>
#include <fstream>
#include <iostream>
#include <ios>
#include <string.h>
#include <strings.h>
#include <string>
#include <sstream>
#include <Errors/ErrorConsole.h>
#include <Errors/Errors.h>
#include <Errors/Exceptions.h>
#include <Config/SBQLConfig.h>
#include <Util/Locker.h>

using namespace std;
using namespace Config;
using namespace Util;

namespace Errors {

	bool ErrorConsole::use_log_file;
	bool ErrorConsole::serr;
	auto_ptr<ofstream> ErrorConsole::console_file;
	auto_ptr<SBQLConfig> ErrorConsole::conf_file;
	VerbosityLevel log_file_level, serr_level;
	pthread_mutex_t ErrorConsole::write_lock = PTHREAD_MUTEX_INITIALIZER;
	map<ConsoleInstance, ErrorConsole* > ErrorConsole::modules;
	

	const string ErrorConsole::names[] = {
		"ARIESLog",
		"IndexManager",
		"InterfaceMaps",
		"LockManager",
		"Log",
		"OuterSchemas",
		"QueryExecutor",
		"QueryParser",
		"Server",
		"Store",
		"Store: Classes",
		"Store: File",
		"Store: Interfaces",
		"Store: NamedItems",
		"Store: NamedRoots",
		"Store: Schemas",
		"Store: Views",
		"TransactionManager",
		"TypeChecker"
	};

	const string ErrorConsole::verb_debug_name = "DEBUG";
	const string ErrorConsole::verb_info_name = "INFO";
	const string ErrorConsole::verb_warning_name = "WARNING";
	const string ErrorConsole::verb_error_name = "ERROR";
	const string ErrorConsole::verb_severe_error_name = "SEVERE_ERROR";

	void ErrorConsole::init_static()
	{
		if (conf_file.get())
			return;
		conf_file = auto_ptr<SBQLConfig>(new SBQLConfig("Errors"));
		if (conf_file->getBool("stderr", serr))
			serr = true;
		log_file_level = parse_verbosity("logfile_verbosity");
		serr_level = parse_verbosity("serr_verbosity");
		string f;
		use_log_file = !conf_file->getString("logfile", f) &&
			f.compare("OFF") && f.compare("off");
		if (use_log_file) {
			console_file = auto_ptr<ofstream>(new ofstream(f.c_str(), ios::app));
			if (!console_file->is_open())
				throw LoximException(ENoFile | ErrErrors);
		}

	}

	VerbosityLevel ErrorConsole::parse_verbosity(const string &property)
	{
		string s_l;
		if (conf_file->getString(property, s_l))
			return V_DEFAULT;
		if (!strcasecmp(s_l.c_str(), "severe"))
			return V_SEVERE_ERROR;
		if (!strcasecmp(s_l.c_str(), "error"))
			return V_ERROR;
		if (!strcasecmp(s_l.c_str(), "warning"))
			return V_WARNING;
		if (!strcasecmp(s_l.c_str(), "info"))
			return V_INFO;
		if (!strcasecmp(s_l.c_str(), "debug"))
			return V_DEBUG;
		cerr << "Unknown verbosity level for " + property + 
			", falling back to default" << endl;
		return V_DEFAULT;
	}

	const string &ErrorConsole::verbosity_name(VerbosityLevel l)
	{
		switch (l){
			case V_DEBUG: return verb_debug_name;
			case V_INFO: return verb_info_name;
			case V_WARNING: return verb_warning_name;
			case V_ERROR: return verb_error_name;
			case V_SEVERE_ERROR: return verb_severe_error_name;
		}
		/* not sure if it's the right decision */
		return verb_debug_name;
	}

	const SBQLConfig &ErrorConsole::get_config()
	{
		init_static();
		return *conf_file;
	}

	ErrorConsole::ErrorConsole(ConsoleInstance instance) : instance(instance)
	{
		init_static();
	}


	void ErrorConsole::put_string(const string &error_msg, VerbosityLevel l)
	{
		const string &instance_name(names[instance]);
		Locker lock(write_lock);
		if (use_log_file && log_file_level >= l) {
			*console_file << verbosity_name(l) << ": " << instance_name << ": " << error_msg << endl;
			console_file->flush();
		}
		if (serr && serr_level >= l) {
			cerr << verbosity_name(l) << ": " << instance_name << ": " << error_msg << endl;
			cerr.flush();
		}
	}

	void ErrorConsole::put_errno(int error, VerbosityLevel l)
	{
		const string &src_mod = err_module_desc(error);
		string str = SBQLstrerror(error);
		stringstream ss;
		ss << src_mod << " said: " << str << " (errno: " 
				<< (error & ~ErrAllModules) << ")";
		put_string(ss.str(), l);
	}


	ErrorConsole& ErrorConsole::operator<<(int error)
	{
		put_errno(error);
		return *this;
	}

	ErrorConsole& ErrorConsole::operator<<(const string &error_msg)
	{
		put_string(error_msg);
		return *this;
	}

	ErrorConsole& ErrorConsole::printf(const char *format, ...)
	{
		//CAUTION!!! This code is duplicated below. It's ugly, but most
		//portable. Compilers have different implementations of va_list,
		//so it is not easy to pass the parameters to a helper function.
#define BUF_SIZE 1024
		char str[BUF_SIZE];
		va_list ap;
		string dest_mod;

		va_start(ap, format);
		vsnprintf(str, BUF_SIZE, format, ap);
		va_end(ap);
		
		//Being a little bit paranoid won't hurt ;)
		str[BUF_SIZE - 1] = 0;
		put_string(str, V_DEPRECATED);
#undef BUF_SIZE
		return *this;
	}

	ErrorConsoleAdapter ErrorConsole::operator()(VerbosityLevel l)
	{
		return ErrorConsoleAdapter(*this, l);
	}

	ErrorConsole::~ErrorConsole()
	{
	}

	ErrorConsole &ErrorConsole::get_instance(ConsoleInstance mod)
	{
		Locker l(write_lock);
		map<ConsoleInstance, ErrorConsole*>::const_iterator
			i = modules.find(mod);
		if (i != modules.end()){
			return *(i->second);
		}
		return *(modules[mod] = new ErrorConsole(mod));

	}



	ErrorConsoleAdapter::ErrorConsoleAdapter(ErrorConsole &cons, VerbosityLevel l) :
		cons(cons), l(l)
	{
	}

	
	ErrorConsoleAdapter &ErrorConsoleAdapter::operator<<(int error)
	{
		cons.put_errno(error, l);
		return *this;
	}

	
	ErrorConsoleAdapter &ErrorConsoleAdapter::operator<<(const string &error)
	{
		cons.put_string(error, l);
		return *this;
	}
	
	void ErrorConsoleAdapter::print(const string &msg)
	{
		cons.put_string(msg, l);
	}

	void ErrorConsoleAdapter::printf(const char *format, ...)
	{
		//CAUTION!!! This code is duplicated above. It's ugly, but most
		//portable. Compilers have different implementations of va_list,
		//so it is not easy to pass the parameters to a helper function.
#define BUF_SIZE 1024
		char str[BUF_SIZE];
		va_list ap;
		string dest_mod;

		va_start(ap, format);
		vsnprintf(str, BUF_SIZE, format, ap);
		va_end(ap);
		
		//Being a little bit paranoid won't hurt ;)
		str[BUF_SIZE - 1] = 0;
		cons.put_string(str, l);
#undef BUF_SIZE
	}
}

