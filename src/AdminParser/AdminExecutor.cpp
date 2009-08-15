#include <pthread.h>
#include <string>
#include <memory>
#include <AdminParser/AdminExecutor.h>
#include <Errors/Errors.h>
#include <Errors/Exceptions.h>
#include <Util/Concurrency.h>

using namespace std;
using namespace Util;
using namespace Errors;

namespace AdminParser{

	AdminExecutor *AdminExecutor::get_instance()
	{
		pthread_mutex_lock(&instance_mutex);
		if (!instance)
			instance = new AdminExecutor();
		pthread_mutex_unlock(&instance_mutex);
		return instance;
	}

	int AdminExecutor::execute(std::string stmt, Server::Session *session)
	{
		Mutex::Locker l(exec_mutex);
		auto_ptr<AdminExecutableTreeNode> node = parser.parse(stmt);
		node->execute(session);
		return 0;
	}

	AdminExecutor::AdminExecutor()
	{
	}

	AdminExecutor::~AdminExecutor()
	{
	}

	AdminExecutor *AdminExecutor::instance = 0;
	pthread_mutex_t AdminExecutor::instance_mutex = PTHREAD_MUTEX_INITIALIZER;

}
