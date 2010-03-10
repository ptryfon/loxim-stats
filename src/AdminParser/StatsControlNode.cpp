#include <AdminParser/StatsControlNode.h>
#include <Server/Session.h>
#include <Errors/ErrorConsole.h>
#include <SystemStats/Statistics.h>

using namespace SystemStatsLib;
namespace AdminParser{
    int StatsControlNode::execute(Server::Session *session)
    {
			return -1;
    }
	
	int StatsControlNode::execute_global_start()
	{
		debug_print(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "Global stats start executed\n");
		return 0;
	}
	
	int StatsControlNode::execute_global_stop()
	{
		debug_print(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "Global stats stop executed\n");
				return 0;
	}

	int StatsControlNode::execute_dump_start()
	{
		debug_print(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "Global stats dump start executed\n");

		return 0;
	}
	
	int StatsControlNode::execute_dump_stop()
	{
debug_print(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "Global stats dump stop executed\n");
		return 0;
	}
	
	int StatsControlNode::execute_global_clear()
	{
		debug_print(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "Global stats clear executed\n");
		return 0;
	}

	int StatsControlNode::execute_set_dump_interval()
	{
		debug_print(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "Set dump interval " << number << " executed\n");

		return 0;
	}
}

