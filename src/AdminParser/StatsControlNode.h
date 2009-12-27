#ifndef STATS_CONTROL_NODE_H
#define STATS_CONTROL_NODE_H

#include <AdminParser/AdminExecutableTreeNode.h>
namespace Server {
	class Session;
}

enum StatsControlAction{
	START_GLOBAL,
	STOP_GLOBAL,
	START_DUMP,
	STOP_DUMP,
	CLEAR_GLOBAL,
};

namespace AdminParser{
    class StatsControlNode : public AdminExecutableTreeNode {
		public:
			StatsControlNode(StatsControlAction _action, int _scope): action(_action), scope(_scope) {}
		private:
			virtual int execute(Server::Session *session);
			int execute_global_start(int scope);
			int execute_global_stop(int scope);
			int execute_dump_start(int scope);
			int execute_dump_stop(int scope);
			int execute_global_clear(int scope);
			
			StatsControlAction action;
			int scope;
    };
}

#endif
