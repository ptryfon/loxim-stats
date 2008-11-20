#include <AdminParser/ShutdownTreeNode.h>
#include <iostream>

namespace AdminParser{
    int ShutdownTreeNode::execute(LoximServer::LoximSession *session)
    {
	session->get_server()->shutdown();
	return 0;
    }
}

