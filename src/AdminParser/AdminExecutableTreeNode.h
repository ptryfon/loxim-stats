#ifndef ADMIN_EXECUTABLE_TREE_NODE_H
#define ADMIN_EXECUTABLE_TREE_NODE_H

#include <Server/Session.h>
#include <AdminParser/AdminTreeNode.h>

namespace AdminParser{
    class AdminExecutableTreeNode : public AdminTreeNode {
	public:
	    virtual int execute(Server::Session *session) = 0;
	    virtual bool is_executable();
    };
}

#endif
