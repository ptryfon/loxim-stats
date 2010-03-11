/**
 * StatsDumper.h
 * @author: Pawel Tryfon (pt248444)
 * This file is prividing implementation for the class responsible for dumping statistics
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#include <SystemStats/StatsDumper.h>
#include <SystemStats/Statistics.h>
#include <Config/SBQLConfig.h>
#include <Errors/ErrorConsole.h>

namespace SystemStatsLib {

StatsDumper* StatsDumper::p_instance = NULL;

#define DEFAULT_INTERVAL 5000 //in miliseconds

StatsDumper::StatsDumper(): dump_interval(DEFAULT_INTERVAL), dump_scope(NOTHING)
{	
}

StatsDumper*
StatsDumper::get_instance()
{
	if (p_instance == NULL) {
		p_instance = new StatsDumper();
	}
	return p_instance;
}

/* To make this function visible only inside this file */
namespace {

int
loxim_make_dir(std::string dir)
{
	debug_print(Errors::ErrorConsole::get_instance(Errors::EC_STATS), 
				"Creating dir " << dir << "\n");
	mode_t dir_mode = 0777;
	if (mkdir(dir.c_str(), dir_mode) != 0 && errno != EEXIST) {
		error_print(Errors::ErrorConsole::get_instance(Errors::EC_STATS), 
				"Couldn't create directory and it does not exists: " << dir << "\n");
		return -1;
	}
	return 0;
}

}
 
void
StatsDumper::start_dumping(StatsScope _scope)
{
	const string loxim_default_dump_dir("/tmp/loxim_dump_dir/");
	if (_scope == CONFIG ||_scope == QUERIES)
		return;
		
	Config::SBQLConfig config("statistics");
	string loxim_dump_dir = config.getStringDefault("dump_dir", loxim_default_dump_dir);
	
	if (loxim_make_dir(loxim_dump_dir) != 0)
		return;
	
	StatsScope result = static_cast<StatsScope>(dump_scope | _scope);
	
	if (dump_scope == NOTHING) {
		time(&start_time);
		ostringstream dir;
		dir << loxim_dump_dir << start_time << "/";
		dump_dir = dir.str();
		if (loxim_make_dir(dump_dir) != 0)
			return;
		if ((_scope & STORE) != 0) {
			store_dump_dir = dump_dir + "store/";
			if (loxim_make_dir(store_dump_dir) != 0)
				return;
		}
		if ((_scope & SESSION) != 0) {
			sessions_dump_dir = dump_dir + "sessions/";
			if (loxim_make_dir(sessions_dump_dir) != 0)
				return;
		}
		if ((_scope & TRANSACTION) != 0) {
			transactions_dump_dir = dump_dir + "transactions/";
			if (loxim_make_dir(transactions_dump_dir) != 0)
				return;
		}
		dump_scope = result;
		run();
	}
	else
		dump_scope = result;
}

void
StatsDumper::stop_dumping(StatsScope _scope)
{
	StatsScope result = static_cast<StatsScope>(dump_scope & (~_scope));
	dump_scope = result;
}

void
StatsDumper::start()
{
	debug_print(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "StatsDumper::start was called\n");
	while (dump_scope && !stop) 
	{
		debug_print(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "StatsDumper is running\n");
		
		StatsOutput* output;
		time_t timestamp;
		if ((dump_scope & STORE) != 0) {
			output = Statistics::get_unified_statistics(STORE)->get_stats_output();
			time(&timestamp);
			dump(store_dump_dir, output, timestamp);
			free(output);
		}
		if ((dump_scope & SESSION) != 0) {
			output = Statistics::get_unified_statistics(SESSION)->get_stats_output();
			time(&timestamp);
			dump(sessions_dump_dir, output, timestamp);
			free(output);
		}
		if ((dump_scope & TRANSACTION) != 0) {
			output = Statistics::get_unified_statistics(TRANSACTION)->get_stats_output();
			time(&timestamp);
			dump(transactions_dump_dir, output, timestamp);
			free(output);
		}
		sleep(dump_interval/1000);
		if ((dump_interval % 1000) != 0)
			usleep(1000*(dump_interval % 1000));
	}
}

void 
StatsDumper::dump(std::string dir, StatsOutput* output, time_t timestamp)
{
	debug_print(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "Dumping stats type " << output->stats_id << " from time " << timestamp << " to dir " << dir << "\n");
	std::vector<std::string>::const_iterator names_iter;
	std::vector<int>::const_iterator int_iter;
	fstream file;
	string filename;
	for (names_iter = output->int_names.begin(), int_iter = output->int_stats.begin();
		names_iter != output->int_names.end() && int_iter != output->int_stats.end();
		names_iter++, int_iter++) {
		filename = dir + *names_iter;
		file.open(filename.c_str(), fstream::out | fstream::app);
		if (file.is_open())
			file << timestamp << " " << *int_iter << "\n";
		file.close();
	}
	std::vector<double>::const_iterator double_iter;
	for (names_iter = output->double_names.begin(), double_iter = output->double_stats.begin();
		names_iter != output->double_names.end() && double_iter != output->double_stats.end();
		names_iter++, double_iter++) {
		filename = dir + *names_iter;
		file.open(filename.c_str(), fstream::out | fstream::app);
		if (file.is_open())
			file << timestamp << " " << *double_iter << "\n";
		file.close();
	}
	std::vector<string>::const_iterator string_iter;
	for (names_iter = output->string_names.begin(), string_iter = output->string_stats.begin();
		names_iter != output->string_names.end() && string_iter != output->string_stats.end();
		names_iter++, string_iter++) {
		filename = dir + *names_iter;
		file.open(filename.c_str(), fstream::out | fstream::app);
		if (file.is_open())
			file << timestamp << " " << *string_iter << "\n";
		file.close();
	}
}
	
}
