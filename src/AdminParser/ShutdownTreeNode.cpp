#include <AdminParser/ShutdownTreeNode.h>
#include <Server/Session.h>
#include <iostream>

namespace AdminParser{
    int ShutdownTreeNode::execute(Server::Session *session)
    {
	session->get_server().shutdown();
	return 0;
    }
}

