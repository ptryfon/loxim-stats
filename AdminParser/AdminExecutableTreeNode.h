#ifndef ADMIN_EXECUTABLE_TREE_NODE_H
#define ADMIN_EXECUTABLE_TREE_NODE_H

#include "../LoximServer/LoximSession.h"
#include "AdminTreeNode.h"

namespace AdminParser{
    class AdminExecutableTreeNode : public AdminTreeNode {
	public:
	    virtual int execute(LoximServer::LoximSession *session) = 0;
	    virtual bool is_executable();
    };
}

#endif
