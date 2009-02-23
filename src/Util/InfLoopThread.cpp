#include <Util/InfLoopThread.h>
#include <cassert>

using namespace std;

namespace Util {
	void *InfLoopThread::starter(void *arg)
	{
		InfLoopThread *thread_obj = reinterpret_cast<InfLoopThread*>(arg);
		assert(thread_obj != NULL);
		thread_obj->alive = true;
		thread_obj->pre_loop();
		return NULL;
	}

	InfLoopThread::InfLoopThread() : shutting_down(false), alive(false)
	{
	}

	InfLoopThread::~InfLoopThread()
	{
		kill(true);
	}

	bool InfLoopThread::is_alive()
	{
		return alive;
	}

}

