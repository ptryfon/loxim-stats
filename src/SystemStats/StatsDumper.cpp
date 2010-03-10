/**
 * StatsDumper.h
 * @author: Pawel Tryfon (pt248444)
 * This file is prividing implementation for the class responsible for dumping statistics
 */

namespace SystemStatsLib {

Util::Mutex StatsDumper::dumper_wait_mutex;

StatsDumper&
StatsDumper::get_instance()
{
	if (p_instance.get == NULL) {
		p_instance = std:auto_ptr<StatsDumper> (new StatsDumper());
	}
	return *p_instance;
}

void
StatsDumper::start_dumping(StatsScope _scope)
{
	StatsScope result = dump_scope | _scope;
	dump_scope = result & ALL;
}

void
StatsDumper::stop_dumping(StatsScope _scope)
{
	StatsScope result = dump_scope & (~_scope);
	dump_scope = result;
}

void
StatsDumper::start()
{
	while (dump_scope) 
	{
		
	}
}
	
}