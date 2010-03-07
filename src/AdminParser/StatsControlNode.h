#ifndef STATS_CONTROL_NODE_H
#define STATS_CONTROL_NODE_H

#include <SystemStats/StatsTypes.h>
#include <AdminParser/AdminExecutableTreeNode.h>
namespace Server {
	class Session;
}

namespace AdminParser {

enum StatsControlAction{
	START_GLOBAL,
	STOP_GLOBAL,
	START_DUMP,
	STOP_DUMP,
	CLEAR_GLOBAL,
	SET_DUMP_INTERVAL,
};

    class StatsControlNode : public AdminExecutableTreeNode {
		public:
			StatsControlNode(StatsControlAction _action, SystemStatsLib::StatsScope _scope): action(_action), scope(_scope) {}
			StatsControlNode(StatsControlAction _action, SystemStatsLib::StatsScope _scope, int _number): action(_action), scope(_scope), number(_number) {}
		private:
			virtual int execute(Server::Session *session);
			int execute_global_start();
			int execute_global_stop();
			int execute_dump_start();
			int execute_dump_stop();
			int execute_global_clear();
			int execute_set_dump_interval();
			
			StatsControlAction action;
			SystemStatsLib::StatsScope scope;
			int number;
    };
}

#endif
