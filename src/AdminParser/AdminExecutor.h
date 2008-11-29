#ifndef ADMIN_PARSER_H
#define ADMIN_PARSER_H

#include <string>
#include <AdminParser/AdminLexer.h>
#include <AdminParser/parser.h>
#include <Server/Session.h>
#include <AdminParser/AdminExecutableTreeNode.h>
#include <pthread.h>

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
	    AdminParser *parser;
	    AdminLexer *lexer;
	    
	    /**
	     * for returning results from parser
	     */
	    AdminExecutableTreeNode *node;
    };
}

#endif
