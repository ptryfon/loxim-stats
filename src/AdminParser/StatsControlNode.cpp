#include <AdminParser/StatsControlNode.h>
#include <Server/Session.h>
#include <Errors/ErrorConsole.h>
#include <SystemStats/Statistics.h>

using namespace SystemStatsLib;
namespace AdminParser{
    int StatsControlNode::execute(Server::Session *session)
    {
		switch (action) {
			case START_GLOBAL:
				return execute_global_start();
			case STOP_GLOBAL:
				return execute_global_stop();
			case START_DUMP:
				return execute_dump_start();
			case STOP_DUMP:
				return execute_dump_stop();
			case CLEAR_GLOBAL:
				return execute_global_clear();
			case SET_DUMP_INTERVAL:
				return execute_set_dump_interval();
			default:
				return -1;
		}
    }
	
	int StatsControlNode::execute_global_start()
	{
		debug_print(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "Global stats start executed\n");
		switch (scope) {
			case ALL:
				Statistics::get_statistics(pthread_self()).start_store_stats();
				Statistics::get_statistics(pthread_self()).start_sessions_stats();
				Statistics::get_statistics(pthread_self()).start_transactions_stats();
				Statistics::get_statistics(pthread_self()).start_queries_stats();
				Statistics::get_statistics(pthread_self()).start_config_stats();
				break;
			case STORE:
				Statistics::get_statistics(pthread_self()).start_store_stats();
				break;
			case SESSION:
				Statistics::get_statistics(pthread_self()).start_sessions_stats();
				break;
			case TRANSACTION:
				Statistics::get_statistics(pthread_self()).start_transactions_stats();
				break;
			case QUERIES:
				Statistics::get_statistics(pthread_self()).start_queries_stats();
				break;
			case CONFIG:
				Statistics::get_statistics(pthread_self()).start_config_stats();
				break;
			default:
				return -1;
		}
		return 0;
	}
	
	int StatsControlNode::execute_global_stop()
	{
		debug_print(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "Global stats stop executed\n");
		switch (scope) {
			case ALL:
				Statistics::get_statistics(pthread_self()).stop_store_stats();
				Statistics::get_statistics(pthread_self()).stop_sessions_stats();
				Statistics::get_statistics(pthread_self()).stop_transactions_stats();
				Statistics::get_statistics(pthread_self()).stop_queries_stats();
				Statistics::get_statistics(pthread_self()).stop_config_stats();
				break;
			case STORE:
				Statistics::get_statistics(pthread_self()).stop_store_stats();
				break;
			case SESSION:
				Statistics::get_statistics(pthread_self()).stop_sessions_stats();
				break;
			case TRANSACTION:
				Statistics::get_statistics(pthread_self()).stop_transactions_stats();
				break;
			case QUERIES:
				Statistics::get_statistics(pthread_self()).stop_queries_stats();
				break;
			case CONFIG:
				Statistics::get_statistics(pthread_self()).stop_config_stats();
				break;
			default:
				return -1;
		}

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

