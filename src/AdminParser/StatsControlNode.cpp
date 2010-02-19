#include <AdminParser/StatsControlNode.h>
#include <Server/Session.h>
#include <Errors/ErrorConsole.h>

namespace AdminParser{
    int StatsControlNode::execute(Server::Session *session)
    {
		switch (action) {
			case START_GLOBAL:
				return execute_global_start(scope);
			case STOP_GLOBAL:
				return execute_global_start(scope);
			case START_DUMP:
				return execute_dump_start(scope);
			case STOP_DUMP:
				return execute_dump_stop(scope);
			case CLEAR_GLOBAL:
				return execute_global_clear(scope);
			default:
				return -1;
		}
    }
	
	int StatsControlNode::execute_global_start(int scope)
	{
		debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "Global stats start executed\n");
		return 0;
	}
	
	int StatsControlNode::execute_global_stop(int scope)
	{
		debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "Global stats stop executed\n");
		return 0;
	}
	
	int StatsControlNode::execute_dump_start(int scope)
	{
		debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "Global stats dump start executed\n");

		return 0;
	}
	
	int StatsControlNode::execute_dump_stop(int scope)
	{
debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "Global stats dump stop executed\n");
		return 0;
	}
	
	int StatsControlNode::execute_global_clear(int scope)
	{
		debug_printf(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "Global stats clear executed\n");
		return 0;
	}
}

