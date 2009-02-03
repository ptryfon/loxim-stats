#ifndef SHUTDOWN_TREE_NODE_H
#define SHUTDOWN_TREE_NODE_H

#include <AdminParser/AdminExecutableTreeNode.h>
namespace Server {
	class Session;
}
namespace AdminParser{
    class ShutdownTreeNode : public AdminExecutableTreeNode {
	virtual int execute(Server::Session *session);
    };
}

#endif
