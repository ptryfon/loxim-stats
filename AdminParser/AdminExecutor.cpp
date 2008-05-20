#include "AdminExecutor.h"
#include <string>
#include "../Errors/Errors.h"
#include <pthread.h>

AdminParser::AdminExecutor *AdminParser::AdminExecutor::get_instance()
{
    pthread_mutex_lock(&instance_mutex);
    if (!instance)
	instance = new AdminExecutor();
    pthread_mutex_unlock(&instance_mutex);
    return instance;
}

int AdminParser::AdminExecutor::execute(std::string stmt, LoximServer::LoximSession *session)
{
    pthread_mutex_lock(&exec_mutex);
    stringstream stream(stringstream::in | stringstream::out);
    stream << stmt;
    lexer->switch_streams(&stream, 0);
    int res = parser->parse();
    if (res){
	pthread_mutex_unlock(&exec_mutex);
	return ENotParsed;
    }
    res = node->execute(session);
    delete node;
    pthread_mutex_unlock(&exec_mutex);
    return res;
}

AdminParser::AdminExecutor::AdminExecutor()
{
    lexer = new AdminLexer();
    parser = new AdminParser::AdminParser(lexer, &node);
    pthread_mutex_init(&exec_mutex, 0);
}

AdminParser::AdminExecutor::~AdminExecutor()
{
    delete parser;
    delete lexer;
}

AdminParser::AdminExecutor *AdminParser::AdminExecutor::instance = 0;
pthread_mutex_t AdminParser::AdminExecutor::instance_mutex = PTHREAD_MUTEX_INITIALIZER;
