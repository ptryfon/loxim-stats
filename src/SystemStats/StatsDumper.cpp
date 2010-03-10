/**
 * StatsDumper.h
 * @author: Pawel Tryfon (pt248444)
 * This file is prividing implementation for the class responsible for dumping statistics
 */

#include <SystemStats/StatsDumper.h>
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

void
StatsDumper::start_dumping(StatsScope _scope)
{
	StatsScope result = static_cast<StatsScope>(dump_scope | _scope);
	if (dump_scope == NOTHING) {
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
	while (dump_scope) 
	{
		debug_print(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "StatsDumper is running\n");
		usleep(1000*dump_interval);
	}
}
	
}