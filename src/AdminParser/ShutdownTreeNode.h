#ifndef SHUTDOWN_TREE_NODE_H
#define SHUTDOWN_TREE_NODE_H

#include <AdminParser/AdminExecutableTreeNode.h>
#include <LoximServer/LoximSession.h>

namespace AdminParser{
    class ShutdownTreeNode : public AdminExecutableTreeNode {
	virtual int execute(LoximServer::LoximSession *session);
    };
}

#endif
