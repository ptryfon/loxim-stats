#ifndef ADMIN_EXECUTOR_H
#define ADMIN_EXECUTOR_H

#include <string>
#include <AdminParser/AdminExecutableTreeNode.h>
#include <AdminParser/AdminParser.h>
#include <AdminParser/AdminLexer.h>
#include <pthread.h>

namespace Server {
	class Session;
}

namespace AdminParser{

    class AdminExecutor{
	public:
	    static AdminExecutor *get_instance();
	    int execute(std::string, Server::Session *session);
	private:
	    AdminExecutor();
	    ~AdminExecutor();
	    static AdminExecutor *instance;
	    pthread_mutex_t exec_mutex;
	    static pthread_mutex_t instance_mutex;
	    ::AdminParser::AdminParser parser;
	    
	    /**
	     * for returning results from parser
	     */
	    AdminExecutableTreeNode *node;
    };
}

#endif
